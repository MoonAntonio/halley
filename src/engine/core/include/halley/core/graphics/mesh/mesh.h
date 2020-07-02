#pragma once

#include "halley/resources/resource.h"
#include "halley/maths/vector4.h"
#include "../graphics_enums.h"

namespace Halley {
	class ResourceLoader;
	class Material;

	struct VertexData
	{
		Vector4f pos;
		Vector4f normal;
		Vector4f colour;
		Vector4f texCoord0;
	};

    class Mesh final : public Resource {
    public:
		Mesh();
		explicit Mesh(ResourceLoader& loader);

		static std::unique_ptr<Mesh> loadResource(ResourceLoader& loader);
		constexpr static AssetType getAssetType() { return AssetType::Mesh; }

        uint32_t getNumVertices() const;
		gsl::span<const Byte> getVertexData() const;
		gsl::span<const IndexType> getIndices() const;
        std::shared_ptr<const Material> getMaterial() const;

		void setVertices(size_t num, Bytes vertexData);
		void setIndices(std::vector<IndexType> indices);
		void setMaterialName(String name);
		void setTextureNames(std::vector<String> textureNames);

		void serialize(Serializer& deserializer) const;
		void deserialize(Deserializer& deserializer);

    private:
		uint32_t numVertices = 0;
        Bytes vertexData;
        std::vector<IndexType> indices;

		String materialName;
		std::vector<String> textureNames;
		std::shared_ptr<Material> material;
    };
}
