#pragma once

#include <halley/maths/vector2.h>
#include <halley/maths/rect.h>
#include <memory>
#include <halley/resources/resource.h>
#include <halley/text/halleystring.h>
#include <halley/data_structures/hash_map.h>
#include <gsl/span>
#include "halley/maths/vector4.h"

namespace Halley
{
	class Resources;
	class Serializer;
	class Deserializer;
	class ResourceDataStatic;
	class Texture;
	class ResourceLoader;

	class SpriteSheetEntry
	{
	public:
		Vector2f pivot;
		Vector2i origPivot;
		Vector2f size;
		Rect4f coords;
		Vector4s trimBorder;
		Vector4s slices;
		int duration = 0;
		bool rotated = false;
		bool sliced = false;

		void serialize(Serializer& s) const;
		void deserialize(Deserializer& s);
	};

	class SpriteSheetFrameTag
	{
	public:
		String name;
		int from = 0;
		int to = 0;

		void serialize(Serializer& s) const;
		void deserialize(Deserializer& s);
	};
	
	class SpriteSheet : public Resource
	{
	public:
		const std::shared_ptr<const Texture>& getTexture() const;
		const SpriteSheetEntry& getSprite(const String& name) const;
		const SpriteSheetEntry& getSprite(size_t idx) const;

		const std::vector<SpriteSheetFrameTag>& getFrameTags() const;
		std::vector<String> getSpriteNames() const;

		size_t getSpriteCount() const;
		size_t getIndex(const String& name) const;
		bool hasSprite(const String& name) const;

		void loadJson(gsl::span<const gsl::byte> data);

		void addSprite(String name, const SpriteSheetEntry& sprite);
		void setTextureName(String name);

		static std::unique_ptr<SpriteSheet> loadResource(ResourceLoader& loader);
		constexpr static AssetType getAssetType() { return AssetType::SpriteSheet; }
		void reload(Resource&& resource) override;

		void serialize(Serializer& s) const;
		void deserialize(Deserializer& s);

	private:
		Resources* resources = nullptr;

		mutable std::shared_ptr<const Texture> texture;
		std::vector<SpriteSheetEntry> sprites;
		HashMap<String, uint32_t> spriteIdx;
		std::vector<SpriteSheetFrameTag> frameTags;
		String textureName;

		void loadTexture(Resources& resources) const;
	};

	class SpriteResource : public Resource
	{
	public:
		SpriteResource(std::shared_ptr<const SpriteSheet> spriteSheet, size_t idx);

		const SpriteSheetEntry& getSprite() const;
		size_t getIdx() const;
		std::shared_ptr<const SpriteSheet> getSpriteSheet() const;

		constexpr static AssetType getAssetType() { return AssetType::Sprite; }
		static std::unique_ptr<SpriteResource> loadResource(ResourceLoader& loader);
		void reload(Resource&& resource) override;

	private:
		std::weak_ptr<const SpriteSheet> spriteSheet;
		size_t idx = -1;
	};
}
