#include <halley.hpp>
using namespace Halley;

#include "halley/entity/components/transform_2d_component.h"
#include "components/camera_component.h"
#include "components/sprite_component.h"
#include "components/sprite_animation_component.h"
#include "components/text_label_component.h"

// System factory functions


using SystemFactoryPtr = System* (*)();
using SystemFactoryMap = HashMap<String, SystemFactoryPtr>;

static SystemFactoryMap makeSystemFactories() {
	SystemFactoryMap result;
	return result;
}


using ComponentFactoryPtr = std::function<CreateComponentFunctionResult(EntityFactory&, EntityRef&, const ConfigNode&)>;
using ComponentFactoryMap = HashMap<String, ComponentFactoryPtr>;

static ComponentFactoryMap makeComponentFactories() {
	ComponentFactoryMap result;
	result["Transform2D"] = [] (EntityFactory& factory, EntityRef& e, const ConfigNode& node) -> CreateComponentFunctionResult { return factory.createComponent<Transform2DComponent>(e, node); };
	result["Camera"] = [] (EntityFactory& factory, EntityRef& e, const ConfigNode& node) -> CreateComponentFunctionResult { return factory.createComponent<CameraComponent>(e, node); };
	result["Sprite"] = [] (EntityFactory& factory, EntityRef& e, const ConfigNode& node) -> CreateComponentFunctionResult { return factory.createComponent<SpriteComponent>(e, node); };
	result["SpriteAnimation"] = [] (EntityFactory& factory, EntityRef& e, const ConfigNode& node) -> CreateComponentFunctionResult { return factory.createComponent<SpriteAnimationComponent>(e, node); };
	result["TextLabel"] = [] (EntityFactory& factory, EntityRef& e, const ConfigNode& node) -> CreateComponentFunctionResult { return factory.createComponent<TextLabelComponent>(e, node); };
	return result;
}

namespace Halley {
	std::unique_ptr<System> createSystem(String name) {
		static SystemFactoryMap factories = makeSystemFactories();
		auto result = factories.find(name);
		if (result == factories.end()) {
			throw Exception("System not found: " + name, HalleyExceptions::Entity);
		}
		return std::unique_ptr<System>(result->second());
	}

   CreateComponentFunctionResult createComponent(EntityFactory& factory, const String& name, EntityRef& entity, const ConfigNode& componentData) {
		static ComponentFactoryMap factories = makeComponentFactories();
		auto result = factories.find(name);
		if (result == factories.end()) {
			throw Exception("Component not found: " + name, HalleyExceptions::Entity);
		}
		return result->second(factory, entity, componentData);
   }
}
