#pragma once
#include "halley/resources/metadata.h"
#include "halley/file/path.h"
#include "halley/data_structures/maybe.h"

namespace Halley {
    class MetadataImporter {
    public:
	    static void loadMetaData(Metadata& meta, const Path& path, bool isDirectoryMeta, const Path& inputFilePath);
	    static Metadata getMetaData(const Path& inputFilePath, std::optional<Path> dirMetaPath, std::optional<Path> privateMetaPath);
    };
}
