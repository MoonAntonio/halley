#include "graphics/sprite/sprite_sheet.h"
#include "halley/core/graphics/texture.h"
#include "halley/core/api/halley_api.h"
#include "resources/resources.h"
#include "halley/file_formats/json/json.h"
#include <halley/file_formats/json_file.h>
#include "halley/bytes/byte_serializer.h"

using namespace Halley;

template <typename T>
static T readRect(JSONValue value)
{
	return T(value["x"], value["y"], value["w"], value["h"]);
}

template <typename T>
static T readVector(JSONValue value)
{
	return T(value["x"], value["y"]);
}

template <typename T>
static T readSize(JSONValue value)
{
	return T(value["w"], value["h"]);
}

void SpriteSheetEntry::serialize(Serializer& s) const
{
	s << pivot;
	s << origPivot;
	s << size;
	s << coords;
	s << duration;
	s << rotated;
	s << trimBorder;
	s << slices;
}

void SpriteSheetEntry::deserialize(Deserializer& s)
{
	s >> pivot;
	s >> origPivot;
	s >> size;
	s >> coords;
	s >> duration;
	s >> rotated;
	s >> trimBorder;
	s >> slices;
}

void SpriteSheetFrameTag::serialize(Serializer& s) const
{
	s << name;
	s << to;
	s << from;
}

void SpriteSheetFrameTag::deserialize(Deserializer& s)
{
	s >> name;
	s >> to;
	s >> from;
}

const std::shared_ptr<const Texture>& SpriteSheet::getTexture() const
{
	Expects(resources != nullptr);
	if (!texture) {
		loadTexture(*resources);
	}
	return texture;
}

const SpriteSheetEntry& SpriteSheet::getSprite(const String& name) const
{
	return getSprite(getIndex(name));
}

const SpriteSheetEntry& SpriteSheet::getSprite(size_t idx) const
{
	return sprites[idx];
}

const std::vector<SpriteSheetFrameTag>& SpriteSheet::getFrameTags() const
{
	return frameTags;
}

std::vector<String> SpriteSheet::getSpriteNames() const
{
	std::vector<String> result;
	for (auto& f: spriteIdx) {
		result.push_back(f.first);
	}
	return result;
}

size_t SpriteSheet::getSpriteCount() const
{
	return sprites.size();
}

size_t SpriteSheet::getIndex(const String& name) const
{
	auto iter = spriteIdx.find(name);
	if (iter == spriteIdx.end()) {
		String names = "";
		bool first = true;
		for (auto& f: spriteIdx) {
			if (first) {
				first = false;
				names += "\"";
			} else {
				names += "\", \"";
			}
			names += f.first;
		}
		if (!spriteIdx.empty()) {
			names += "\"";
		}
		throw Exception("Spritesheet does not contain sprite \"" + name + "\".\nSprites: { " + names + " }.", HalleyExceptions::Resources);
	} else {
		return size_t(iter->second);
	}
}

bool SpriteSheet::hasSprite(const String& name) const
{
	return spriteIdx.find(name) != spriteIdx.end();
}

std::unique_ptr<SpriteSheet> SpriteSheet::loadResource(ResourceLoader& loader)
{
	auto result = std::make_unique<SpriteSheet>();
	result->resources = &loader.getResources();
	auto data = loader.getStatic();
	Deserializer s(data->getSpan());
	result->deserialize(s);

	return result;
}

void SpriteSheet::loadTexture(Resources& resources) const
{
	texture = resources.get<Texture>(textureName);
}

void SpriteSheet::addSprite(String name, const SpriteSheetEntry& sprite)
{
	sprites.push_back(sprite);
	spriteIdx[name] = uint32_t(sprites.size() - 1);
}

void SpriteSheet::setTextureName(String name)
{
	textureName = name;
}

void SpriteSheet::reload(Resource&& resource)
{
	*this = std::move(dynamic_cast<SpriteSheet&>(resource));
}

void SpriteSheet::serialize(Serializer& s) const
{
	s << textureName;
	s << sprites;
	s << spriteIdx;
	s << frameTags;
}

void SpriteSheet::deserialize(Deserializer& s)
{
	s >> textureName;
	s >> sprites;
	s >> spriteIdx;
	s >> frameTags;
}

void SpriteSheet::loadJson(gsl::span<const gsl::byte> data)
{
	auto src = reinterpret_cast<const char*>(data.data());

	// Parse json
	Json::Reader reader;
	JSONValue root;
	reader.parse(src, src + data.size(), root);

	// Read Metadata
	auto metadataNode = root["meta"];
	Vector2f scale = Vector2f(1, 1);
	textureName = "";
	if (metadataNode) {
		if (metadataNode["image"]) {
			textureName = metadataNode["image"].asString();
			Vector2f textureSize = readSize<Vector2f>(metadataNode["size"]);
			scale = Vector2f(1.0f / textureSize.x, 1.0f / textureSize.y);
		}
		if (metadataNode["frameTags"]) {
			for (auto& frameTag: metadataNode["frameTags"]) {
				frameTags.push_back(SpriteSheetFrameTag());
				auto& f = frameTags.back();
				f.name = frameTag["name"].asString();
				f.from = frameTag["from"].asInt();
				f.to = frameTag["to"].asInt();
			}
		}
	}

	// Read sprites
	auto frames = root["frames"];
	for (auto iter = frames.begin(); iter != frames.end(); ++iter) {
		auto sprite = *iter;

		SpriteSheetEntry entry;
		entry.rotated = sprite["rotated"].asBool();
		
		Rect4f frame = readRect<Rect4f>(sprite["frame"]);
		Vector2f size = frame.getSize();
		Vector2f texSize = size;
		if (entry.rotated) {
			std::swap(texSize.x, texSize.y);
		}
		entry.coords = Rect4f(frame.getTopLeft() * scale, texSize.x * scale.x, texSize.y * scale.y);
		entry.size = size;

		const Vector2i sourceSize = readSize<Vector2i>(sprite["sourceSize"]);
		const Rect4i spriteSourceSize = readRect<Rect4i>(sprite["spriteSourceSize"]);
		Vector2f rawPivot = readVector<Vector2f>(sprite["pivot"]);
		Vector2i pivotPos = Vector2i(int(rawPivot.x * sourceSize.x + 0.5f), int(rawPivot.y * sourceSize.y + 0.5f));
		Vector2i newPivotPos = pivotPos - spriteSourceSize.getTopLeft();
		entry.pivot = Vector2f(newPivotPos) / Vector2f(spriteSourceSize.getSize());

		if (sprite["duration"]) {
			entry.duration = sprite["duration"].asInt();
		}
		
		addSprite(iter.memberName(), entry);
	}	
}


SpriteResource::SpriteResource(std::shared_ptr<const SpriteSheet> spriteSheet, size_t idx)
	: spriteSheet(spriteSheet)
	, idx(idx)
{
}

const SpriteSheetEntry& SpriteResource::getSprite() const
{
	return getSpriteSheet()->getSprite(idx);
}

size_t SpriteResource::getIdx() const
{
	return idx;
}

std::shared_ptr<const SpriteSheet> SpriteResource::getSpriteSheet() const
{
	return spriteSheet.lock();
}

std::unique_ptr<SpriteResource> SpriteResource::loadResource(ResourceLoader& loader)
{
	throw Exception("Not implemented.", HalleyExceptions::Resources);
}

void SpriteResource::reload(Resource&& resource)
{
	*this = std::move(dynamic_cast<SpriteResource&>(resource));
}
