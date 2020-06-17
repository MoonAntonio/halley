#pragma once
#include "halley/tools/tasks/editor_task.h"
#include "halley/file/path.h"
#include "import_assets_database.h"
#include <vector>
#include <set>

namespace Halley
{
	class Project;
	
	class ImportAssetsTask : public EditorTask
	{
	public:
		ImportAssetsTask(String taskName, ImportAssetsDatabase& db, const AssetImporter& importer, Path assetsPath, Vector<ImportAssetsDatabaseEntry> files, std::vector<String> deletedAssets, Project& project, bool packAfter);

	protected:
		void run() override;

	private:
		ImportAssetsDatabase& db;
		const AssetImporter& importer;
		Path assetsPath;
		Project& project;
		const bool packAfter;

		Vector<ImportAssetsDatabaseEntry> files;
		std::vector<String> deletedAssets;
		std::set<String> outputAssets;
		
		std::atomic<int64_t> totalImportTime;
		std::atomic<size_t> assetsImported{};
		size_t assetsToImport{};

		std::mutex mutex;
		
		std::string curFileLabel;

		bool importAsset(ImportAssetsDatabaseEntry& asset);

		std::vector<Path> loadFont(const ImportAssetsDatabaseEntry& asset, Path dstDir);
		std::vector<Path> genericImporter(const ImportAssetsDatabaseEntry& asset, Path dstDir);
	};
}
