#include "halley/tools/assets/import_assets_database.h"
#include "halley/tools/project/project.h"
#include "halley/tools/file/filesystem.h"
#include <set>

using namespace Halley;

Project::Project(const String& platform, Path projectRootPath, Path halleyRootPath)
	: platform(platform)
	, rootPath(projectRootPath)
	, halleyRootPath(halleyRootPath)
{
	importAssetsDatabase = std::make_unique<ImportAssetsDatabase>(getAssetsPath(), getAssetsPath() / "import.db", getAssetsPath() / "assets.db");
	codegenDatabase = std::make_unique<ImportAssetsDatabase>(getGenPath(), getGenPath() / "import.db", getGenPath() / "assets.db");

	initialisePlugins();
	assetImporter = std::make_unique<AssetImporter>(*this, std::vector<Path>{getSharedAssetsSrcPath(), getAssetsSrcPath()});
}

Project::~Project() = default;

Path Project::getAssetsPath() const
{
	return rootPath / "bin" / "assets";
}

Path Project::getAssetsSrcPath() const
{
	return rootPath / "assets_src";
}

Path Project::getSharedAssetsSrcPath() const
{
	return halleyRootPath / "shared_assets";
}

Path Project::getGenPath() const
{
	return rootPath / "gen";
}

Path Project::getGenSrcPath() const
{
	return rootPath / "gen_src";
}

ImportAssetsDatabase& Project::getImportAssetsDatabase()
{
	return *importAssetsDatabase;
}

ImportAssetsDatabase& Project::getCodegenDatabase()
{
	return *codegenDatabase;
}

const AssetImporter& Project::getAssetImporter() const
{
	return *assetImporter;
}

std::unique_ptr<IAssetImporter> Project::getAssetImporterOverride(ImportAssetType type) const
{
	for (auto& plugin: plugins) {
		auto importer = plugin->getAssetImporter(type);
		if (importer) {
			return importer;
		}
	}
	return {};
}

void Project::initialisePlugins()
{
	bool knownPlatform = platform == "pc";

#if _WIN32
#ifndef _DEBUG
	auto pluginPath = halleyRootPath / "plugins";
	auto files = FileSystem::enumerateDirectory(pluginPath);
	for (auto& file: files) {
		// HACK: fix extension for OSX/Linux
		if (file.getExtension() == ".dll") {
			auto plugin = loadPlugin(pluginPath / file);
			if (plugin) {
				auto platforms = plugin->getSupportedPlatforms();
				bool accepted = false;
				for (auto& plat: platforms) {
					accepted = plat == "*" || plat == platform;
					knownPlatform |= plat == platform;
				}
				if (accepted) {
					plugins.emplace_back(std::move(plugin));
				}
			}
		}
	}
#endif
#endif
	
	if (!knownPlatform) {
		throw Exception("Unknown platform: " + platform);
	}
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

Project::HalleyPluginPtr Project::loadPlugin(const Path& path)
{
	// HACK: abstract this/support OSX/support Linux
	String nativePath = path.getString();
	nativePath.replace("/", "\\", true);
	auto module = LoadLibrary(nativePath.c_str());
	if (!module) {
		return {};
	}

	using CreateHalleyPluginFn = IHalleyPlugin*();
	using DestroyHalleyPluginFn = void(IHalleyPlugin*);

	auto createHalleyPlugin = reinterpret_cast<CreateHalleyPluginFn*>(GetProcAddress(module, "createHalleyPlugin"));
	auto destroyHalleyPlugin = reinterpret_cast<DestroyHalleyPluginFn*>(GetProcAddress(module, "destroyHalleyPlugin"));
	if (!createHalleyPlugin || !destroyHalleyPlugin) {
		FreeLibrary(module);
		return {};
	}

	return HalleyPluginPtr(createHalleyPlugin(), [=] (IHalleyPlugin* plugin)
	{
		destroyHalleyPlugin(plugin);
		FreeLibrary(module);
	});
}
