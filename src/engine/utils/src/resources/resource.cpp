#include "halley/resources/resource.h"
#include "halley/support/exception.h"

using namespace Halley;

Resource::~Resource() = default;

void Resource::setMeta(const Metadata& m)
{
	meta = m;
}

const Metadata& Resource::getMeta() const
{
	return meta;
}

void Resource::setAssetId(const String& n)
{
	assetId = n;
}

const String& Resource::getAssetId() const
{
	return assetId;
}

void Resource::onLoaded(Resources& resources)
{
}

int Resource::getAssetVersion() const
{
	return assetVersion;
}

void Resource::reloadResource(Resource&& resource)
{
	++assetVersion;
	resource.assetVersion = assetVersion;
	reload(std::move(resource));
}

void Resource::reload(Resource&& resource)
{
}

ResourceObserver::ResourceObserver()
{
}

ResourceObserver::ResourceObserver(const Resource& res)
{
	startObserving(res);
}

ResourceObserver::~ResourceObserver()
{
	stopObserving();
}

void ResourceObserver::startObserving(const Resource& r)
{
	res = &r;
	assetVersion = res->getAssetVersion();
}

void ResourceObserver::stopObserving()
{
	res = nullptr;
	assetVersion = 0;
}

bool ResourceObserver::needsUpdate() const
{
	return res && res->getAssetVersion() != assetVersion;
}

void ResourceObserver::update()
{
	assetVersion = res->getAssetVersion();
}

const Resource* ResourceObserver::getResourceBeingObserved() const
{
	return res;
}

AsyncResource::AsyncResource() 
	: failed(false)
	, loading(false)
{}

AsyncResource::~AsyncResource()
{
	waitForLoad();
}

void AsyncResource::startLoading()
{
	loading = true;
	failed = false;
}

void AsyncResource::doneLoading()
{
	{
		std::unique_lock<std::mutex> lock(loadMutex);
		loading = false;
	}
	loadWait.notify_all();
}

void AsyncResource::loadingFailed()
{
	failed = true;
	doneLoading();
}

void AsyncResource::waitForLoad() const
{
	if (loading) {
		std::unique_lock<std::mutex> lock(loadMutex);
		while (loading) {
			loadWait.wait(lock);
		}
	}
	if (failed) {
		throw Exception("Resource failed to load.", HalleyExceptions::Resources);
	}
}

bool AsyncResource::isLoaded() const
{
	return !loading;
}
