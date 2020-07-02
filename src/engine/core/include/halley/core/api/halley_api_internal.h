#pragma once
#include <halley/plugin/plugin.h>

#include "core_api.h"
#include "video_api.h"
#include "system_api.h"
#include "input_api.h"
#include "audio_api.h"
#include "network_api.h"
#include "platform_api.h"
#include "movie_api.h"

namespace Halley
{
	class MaterialParameter;

	class HalleyAPIInternal
	{
		friend class Core;
		friend class HalleyAPI;

	public:
		virtual ~HalleyAPIInternal() {}

		virtual void init() = 0;
		virtual void deInit() = 0;

		virtual void onSuspend() {}
		virtual void onResume() {}
	};

	class VideoAPIInternal : public VideoAPI, public HalleyAPIInternal
	{
	public:
		virtual ~VideoAPIInternal() {}

		virtual std::unique_ptr<Painter> makePainter(Resources& resources) = 0;
	};

	class InputAPIInternal : public InputAPI, public HalleyAPIInternal
	{
	public:
		virtual ~InputAPIInternal() {}

		virtual void beginEvents(Time t) = 0;
	};

	class AudioAPIInternal : public AudioAPI, public HalleyAPIInternal
	{
	public:
		virtual ~AudioAPIInternal() {}

		virtual void pump() = 0;
		virtual void setResources(Resources& resources) = 0;
	};

	class AudioOutputAPIInternal : public AudioOutputAPI, public HalleyAPIInternal
	{
	public:
		virtual ~AudioOutputAPIInternal() {}
	};

	class SystemAPIInternal : public SystemAPI, public HalleyAPIInternal
	{
	public:
		virtual ~SystemAPIInternal() {}
		virtual void setEnvironment(Environment*) {}
		virtual void update(Time t) {}
	};

	class CoreAPIInternal : public CoreAPI, public IPluginRegistry {
	public:
		virtual ~CoreAPIInternal() {}
	};

	class NetworkAPIInternal : public NetworkAPI, public HalleyAPIInternal
	{
	public:
		virtual ~NetworkAPIInternal() {}
	};

	class PlatformAPIInternal : public PlatformAPI, public HalleyAPIInternal
	{
	public:
		virtual ~PlatformAPIInternal() {}
	};

	class MovieAPIInternal : public MovieAPI, public HalleyAPIInternal
	{
	public:
		virtual ~MovieAPIInternal() {}
	};
}

