#include "video_opengl.h"
#include <halley/plugin/plugin.h>

namespace Halley {
	
	class OpenGLPlugin : public Plugin {
		HalleyAPIInternal* createAPI(SystemAPI* system) override { return new VideoOpenGL(*system); }
		PluginType getType() override { return PluginType::GraphicsAPI; }
		String getName() override { return "Video/OpenGL"; }
	};
	
}

void initOpenGLPlugin(Halley::IPluginRegistry &registry)
{
	registry.registerPlugin(std::make_unique<Halley::OpenGLPlugin>());
}
