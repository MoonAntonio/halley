/*****************************************************************\
           __
          / /
		 / /                     __  __
		/ /______    _______    / / / / ________   __       __
	   / ______  \  /_____  \  / / / / / _____  | / /      / /
	  / /      | / _______| / / / / / / /____/ / / /      / /
	 / /      / / / _____  / / / / / / _______/ / /      / /
	/ /      / / / /____/ / / / / / / |______  / |______/ /
   /_/      /_/ |________/ / / / /  \_______/  \_______  /
                          /_/ /_/                     / /
			                                         / /
		       High Level Game Framework            /_/

  ---------------------------------------------------------------

  Copyright (c) 2007-2011 - Rodrigo Braz Monteiro.
  This file is subject to the terms of halley_license.txt.

\*****************************************************************/

#include "resources/resource_locator.h"
#include "resources/resource_filesystem.h"
#include <iostream>
#include <set>
#include <halley/support/exception.h>

using namespace Halley;

ResourceLocator::ResourceLocator(SystemAPI& system)
	: system(system)
{
}

void ResourceLocator::add(std::unique_ptr<IResourceLocatorProvider> locator)
{
	auto& db = locator->getAssetDatabase();
	for (auto& asset: db.getAssets()) {
		auto result = locators.find(asset);
		if (result == locators.end() || result->second->getPriority() < locator->getPriority()) {
			locators[asset] = locator.get();
		}
	}
	locatorList.emplace_back(std::move(locator));
}

std::unique_ptr<ResourceData> ResourceLocator::getResource(const String& asset, AssetType type, bool stream)
{
	auto result = locators.find(asset);
	if (result != locators.end()) {
		return result->second->getData(asset, type, stream);
	} else {
		throw Exception("Unable to locate resource: " + asset);
	}
}

std::unique_ptr<ResourceDataStatic> ResourceLocator::getStatic(const String& asset, AssetType type)
{
	auto ptr = dynamic_cast<ResourceDataStatic*>(getResource(asset, type, false).release());
	if (!ptr) {
		throw Exception("Resource " + asset + " obtained, but is not static data. Memory leak has ocurred.");
	}
	return std::unique_ptr<ResourceDataStatic>(ptr);
}

std::unique_ptr<ResourceDataStream> ResourceLocator::getStream(const String& asset, AssetType type)
{
	auto ptr = dynamic_cast<ResourceDataStream*>(getResource(asset, type, true).release());
	if (!ptr) {
		throw Exception("Resource " + asset + " obtained, but is not stream data. Memory leak has ocurred.");
	}
	return std::unique_ptr<ResourceDataStream>(ptr);
}
	
void ResourceLocator::addFileSystem(Path path)
{
	add(std::make_unique<FileSystemResourceLocator>(system, path));
}

const Metadata& ResourceLocator::getMetaData(const String& asset, AssetType type) const
{
	auto result = locators.find(asset);
	if (result != locators.end()) {
		return result->second->getAssetDatabase().getDatabase(type).get(asset).meta;
	} else {
		throw Exception("Unable to locate resource: " + asset);
	}
}
