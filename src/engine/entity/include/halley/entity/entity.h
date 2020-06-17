#pragma once

#include <array>
#include <memory>
#include "component.h"
#include "message.h"
#include "family_mask.h"
#include "entity_id.h"
#include "type_deleter.h"
#include <halley/data_structures/vector.h>

namespace Halley {
	class World;
	class System;

	class MessageEntry
	{
	public:
		std::unique_ptr<Message> msg;
		int type = -1;
		int age = -1;

		MessageEntry() {}
		MessageEntry(std::unique_ptr<Message> msg, int type, int age) : msg(std::move(msg)), type(type), age(age) {}
	};

	class EntityRef;

	class Entity
	{
		friend class World;
		friend class System;
		friend class EntityRef;

	public:
		~Entity();

		template <typename T>
		T* tryGetComponent()
		{
			constexpr int id = FamilyMask::RetrieveComponentIndex<T>::componentIndex;
			for (size_t i = 0; i < components.size(); i++) {
				if (components[i].first == id) {
					return static_cast<T*>(components[i].second);
				}
			}
			return nullptr;
		}

		template <typename T>
		T& getComponent()
		{
			auto value = tryGetComponent<T>();
			if (value) {
				return *value;
			} else {
				throw Exception("Component " + String(typeid(T).name()) + " does not exist in entity.", HalleyExceptions::Entity);
			}
		}

		template <typename T>
		bool hasComponent()
		{
			if (dirty) {
				return getComponent<T>() != nullptr;
			} else {
				return FamilyMask::hasBit(mask, FamilyMask::RetrieveComponentIndex<T>::componentIndex);
			}
		}

		bool needsRefresh() const
		{
			return dirty;
		}

		bool isAlive() const {
			return alive;
		}

		FamilyMaskType getMask() const;
		EntityId getEntityId() const;

		void refresh();
		void destroy();

	private:
		Vector<std::pair<int, Component*>> components;
		Vector<MessageEntry> inbox;
		FamilyMaskType mask;
		EntityId uid;
		int liveComponents = 0;
		bool dirty = false;
		bool alive = true;

		Entity();

		template <typename T>
		Entity& addComponent(World& world, T* component)
		{
			addComponent(component, T::componentIndex);
			TypeDeleter<T>::initialize();

			markDirty(world);
			return *this;
		}

		template <typename T>
		Entity& removeComponent(World& world)
		{
			int id = T::componentIndex;
			for (int i = 0; i < liveComponents; ++i) {
				if (components[i].first == id) {
					removeComponentAt(i);
					markDirty(world);
					return *this;
				}
			}

			return *this;
		}

		void addComponent(Component* component, int id);
		void removeComponentAt(int index);
		void deleteComponent(Component* component, int id);
		void onReady();

		void markDirty(World& world);
	};

	class EntityRef
	{
	public:
		template <typename T>
		EntityRef& addComponent(T&& component)
		{
			static_assert(!std::is_pointer<T>::value, "Cannot pass pointer to component");
			static_assert(!std::is_same<T, Component>::value, "Cannot add base class Component to entity, make sure type isn't being erased");
			static_assert(std::is_base_of<Component, T>::value, "Components must extend the Component class");
			static_assert(!std::is_polymorphic<T>::value, "Components cannot be polymorphic (i.e. they can't have virtual methods)");
			static_assert(std::is_default_constructible<T>::value, "Components must have a default constructor");
			entity.addComponent(world, new T(std::move(component)));
			return *this;
		}

		template <typename T>
		EntityRef& removeComponent()
		{
			entity.removeComponent<T>(world);
			return *this;
		}

		template <typename T>
		T& getComponent()
		{
			return entity.getComponent<T>();
		}

		template <typename T>
		T* tryGetComponent()
		{
			return entity.tryGetComponent<T>();
		}

		EntityId getEntityId() const
		{
			return entity.getEntityId();
		}

		template <typename T>
		bool hasComponent()
		{
			return entity.hasComponent<T>();
		}

	private:
		friend class World;
		EntityRef(Entity& e, World& w)
			: entity(e)
			, world(w)
		{}

		Entity& entity;
		World& world;
	};
}
