#pragma once

#include <memory>
#include <typeinfo>
#include <type_traits>
#include "entity_id.h"
#include "family_mask.h"
#include "family.h"
#include <halley/time/halleytime.h>
#include <halley/text/halleystring.h>
#include <halley/data_structures/mapped_pool.h>
#include <halley/time/stopwatch.h>
#include <halley/data_structures/vector.h>
#include <halley/data_structures/tree_map.h>
#include "service.h"

namespace Halley {
	class ConfigNode;
	class RenderContext;
	class Entity;
	class System;
	class Painter;
	class HalleyAPI;

	class World
	{
	public:
		World(const HalleyAPI* api, bool collectMetrics);
		~World();

		void step(TimeLine timeline, Time elapsed);
		void render(RenderContext& rc) const;
		bool hasSystemsOnTimeLine(TimeLine timeline) const;
		
		int64_t getAverageTime(TimeLine timeline) const;

		System& addSystem(std::unique_ptr<System> system, TimeLine timeline);
		void removeSystem(System& system);
		Vector<System*> getSystems();
		System& getSystem(const String& name);
		Vector<std::unique_ptr<System>>& getSystems(TimeLine timeline);
		const Vector<std::unique_ptr<System>>& getSystems(TimeLine timeline) const;

		Service& addService(std::shared_ptr<Service> service);
		void loadSystems(const ConfigNode& config, std::function<std::unique_ptr<System>(String)> createFunction);

		template <typename T>
		T& getService() const
		{
			static_assert(std::is_base_of<Service, T>::value, "Must extend Service");
			return *dynamic_cast<T*>(&getService(typeid(T).name()));
		}

		EntityRef createEntity();
		void destroyEntity(EntityId id);
		EntityRef getEntity(EntityId id);
		Entity* tryGetEntity(EntityId id);
		size_t numEntities() const;

		void spawnPending(); // Warning: use with care, will invalidate entities

		void onEntityDirty();

		template <typename T>
		Family& getFamily()
		{
			// Disable re-using of families due to optional components messing them up
			/*
			FamilyMaskType mask = T::Type::readMask();
			auto iter = families.find(mask);
			if (iter != families.end()) {
				return *iter->second;
			}
			*/

			auto newFam = std::make_unique<FamilyImpl<T>>();
			Family* newFamPtr = newFam.get();
			onAddFamily(*newFamPtr);
			//families[mask] = std::move(newFam);
			families.emplace_back(std::move(newFam));
			return *newFamPtr;
		}
		
	private:
		const HalleyAPI* api;
		std::array<Vector<std::unique_ptr<System>>, static_cast<int>(TimeLine::NUMBER_OF_TIMELINES)> systems;
		bool collectMetrics = false;
		bool entityDirty = false;
		
		Vector<Entity*> entities;
		Vector<Entity*> entitiesPendingCreation;
		MappedPool<Entity*> entityMap;

		//TreeMap<FamilyMaskType, std::unique_ptr<Family>> families;
		Vector<std::unique_ptr<Family>> families;
		TreeMap<String, std::shared_ptr<Service>> services;

		TreeMap<FamilyMaskType, std::vector<Family*>> familyCache;

		mutable std::array<StopwatchAveraging, 3> timer;

		void allocateEntity(Entity* entity);
		void updateEntities();
		void initSystems();
		void deleteEntity(Entity* entity);

		void updateSystems(TimeLine timeline, Time elapsed);
		void renderSystems(RenderContext& rc) const;
		
		void onAddFamily(Family& family);

		Service& getService(const String& name) const;

		const std::vector<Family*>& getFamiliesFor(const FamilyMaskType& mask);
	};
}
