#pragma once
#include "world.hpp"

class Commands final {
public:
  Commands(World &world) : world_(world) {}
  template <typename... ComponentTypes>
  Commands &Spawn(ComponentTypes &&...components) {
    SpawnAndReturn(std::forward<ComponentTypes>(components)...);
    return *this;
  }

  template <typename... ComponentTypes>
  Entity SpawnAndReturn(ComponentTypes &&...components) {
    EntitySpawnInfo info;
    info.entity = EntityGenerator::Gen(); // gen Entity ID
    if constexpr (sizeof...(components) != 0) {
      doSpawn(info.components, std::forward<ComponentTypes>(components)...);
      spawnEntities_.push_back(info);
    }
    return info.entity;
  }

  template <typename... ComponentTypes>
  Entity SpawnImmediateAndReturn(ComponentTypes &&...components) {
    EntitySpawnInfo info;
    info.entity = EntityGenerator::Gen();
    doSpawn(info.components, std::forward<ComponentTypes>(components)...);

    auto it =
        world_.entities_.emplace(info.entity, World::ComponentContainer{});
    auto &componentContainer = it.first->second;
    for (auto &componentInfo : info.components) {
      componentContainer[componentInfo.index] =
          doSpawnWithoutType(info.entity, componentInfo);
    }

    return info.entity;
  }
  void Execute() {
    for (auto &spawnInfo : spawnEntities_) {
      auto it = world_.entities_.emplace(spawnInfo.entity,
                                         World::ComponentContainer{});
      auto &componentContainer = it.first->second;
      for (auto &componentInfo : spawnInfo.components) {
        componentContainer[componentInfo.index] =
            doSpawnWithoutType(spawnInfo.entity, componentInfo);
      }
    }
  }

private:
  World &world_;
  ////////////////////////////////////////////////////////////////
  using AssignFunc = std::function<void(void *)>;

  struct ComponentSpawnInfo {
    AssignFunc assign;
    World::Pool::CreateFunc create;
    World::Pool::DestroyFunc destroy;
    ComponentTypeID index;
  };

  struct EntitySpawnInfo {
    Entity entity;
    std::vector<ComponentSpawnInfo> components;
  };
  std::vector<EntitySpawnInfo> spawnEntities_;
  template <typename T, typename... Remains>
  void doSpawn(std::vector<ComponentSpawnInfo> &spawnInfo, T &&component,
               Remains &&...remains) {
    ComponentSpawnInfo info;
    info.index = IndexGetter::Get<T>();
    info.create = [](void) -> void * { return new T; };
    info.destroy = [](void *elem) { delete (T *)elem; };
    info.assign = [=, c = std::move(component)](void *elem) mutable {
      *((T *)elem) = std::move(c);
    };
    spawnInfo.push_back(info);

    if constexpr (sizeof...(Remains) != 0) {
      doSpawn(spawnInfo, std::forward<Remains>(remains)...);
    }
  }
  void *doSpawnWithoutType(Entity entity, ComponentSpawnInfo &info) {
    if (auto it = world_.componentMap_.find(info.index);
        it == world_.componentMap_.end()) {
      world_.componentMap_.insert_or_assign(
          info.index, World::ComponentInfo(info.create, info.destroy));
    }
    World::ComponentInfo &componentInfo = world_.componentMap_[info.index];
    void *elem = componentInfo.pool.Create();
    info.assign(elem);
    componentInfo.sparseSet.Add(entity);

    return elem; ///!!!!!!!!!!!!!
  }
};