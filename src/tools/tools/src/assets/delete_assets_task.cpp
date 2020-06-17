#include "halley/tools/assets/delete_assets_task.h"
#include "halley/tools/project/project.h"
#include "halley/tools/assets/import_assets_database.h"
#include <iostream>
#include "halley/tools/file/filesystem.h"

using namespace Halley;

DeleteAssetsTask::DeleteAssetsTask(ImportAssetsDatabase& db, Path assetsPath, Vector<ImportAssetsDatabaseEntry> assets)
	: EditorTask("Deleting assets", true, true)
	, db(db)
	, assetsPath(assetsPath)
	, assets(assets)
{
}

void DeleteAssetsTask::run()
{
	for (auto& asset : assets) {
		if (isCancelled()) {
			break;
		}

		try {
			for (auto& f : asset.outputFiles) {
				for (auto& v: f.platformVersions) {
					FileSystem::remove(assetsPath / v.second.filepath);
				}
			}
			db.markDeleted(asset);
		} catch (std::exception& e) {
			std::cout << "Error removing asset \"" << asset.assetId << "\": " << e.what() << std::endl;
		}
	}
	db.save();
}
