#pragma once

#include "halley/text/halleystring.h"
#include <memory>
#include <vector>

namespace Halley
{
	class HalleyAPIInternal;

	enum class PluginType
	{
		GraphicsAPI,
		AudioAPI,
		InputAPI
	};

	class Plugin
	{
	public:
		virtual ~Plugin() {}

		virtual PluginType getType() = 0;
		virtual String getName() = 0;

		virtual HalleyAPIInternal* createAPI() { return nullptr; }
	};

	class IPluginRegistry
	{
	public:
		virtual ~IPluginRegistry() {}
		virtual void registerPlugin(std::unique_ptr<Plugin> plugin) = 0;
		virtual std::vector<Plugin*> getPlugins(PluginType type) = 0;
	};
}
