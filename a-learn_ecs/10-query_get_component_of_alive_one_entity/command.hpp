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
    info.entity = EntityGenerator::Gen();  // gen Entity ID
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
  template <typename... ComponentTypes>
  Commands &AddComponent(Entity entity, ComponentTypes &&...components) {
    EntitySpawnInfo info;
    info.entity = entity;
    doSpawn(info.components, std::forward<ComponentTypes>(components)...);
    addComponents_.push_back(info);
    return *this;
  }

  template <typename T>
  Commands &DestroyComponent(Entity entity) {
    auto idx = IndexGetter::Get<T>();
    destroyComponents_.emplace_back(ComponentDestroyInfo{entity, idx});

    return *this;
  }

  Commands &DestroyEntity(Entity entity) {
    destroyEntities_.push_back(entity);

    return *this;
  }
  void Execute() {
    for (auto e : destroyEntities_) {
      destroyEntity(e);
    }
    for (const auto &c : destroyComponents_) {
      destroyComponent(c);
    }
    for (auto &spawnInfo : spawnEntities_) {
      auto it = world_.entities_.emplace(spawnInfo.entity,
                                         World::ComponentContainer{});
      auto &componentContainer = it.first->second;
      for (auto &componentInfo : spawnInfo.components) {
        componentContainer[componentInfo.index] =
            doSpawnWithoutType(spawnInfo.entity, componentInfo);
      }
    }
    for (auto &addInfo : addComponents_) {
      auto it = world_.entities_.find(addInfo.entity);
      if (it == world_.entities_.end()) {
        continue;
      }
      auto &componentContainer = it->second;
      for (auto &componentInfo : addInfo.components) {
        componentContainer[componentInfo.index] =
            doSpawnWithoutType(addInfo.entity, componentInfo);
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

    return elem;  ///!!!!!!!!!!!!!
  }
  ///////////////////////////////////////////
  struct EntityDestroyInfo {
    Entity entity;
  };

  struct ComponentDestroyInfo {
    Entity entity;
    ComponentTypeID index;
  };
  std::vector<EntitySpawnInfo> addComponents_;
  std::vector<ComponentDestroyInfo> destroyComponents_;
  std::vector<Entity> destroyEntities_;
  void destroyComponent(const ComponentDestroyInfo &c) {
    if (auto it = world_.entities_.find(c.entity);
        it != world_.entities_.end()) {
      if (auto cit = it->second.find(c.index); cit != it->second.end()) {
        void *component = cit->second;
        it->second.erase(cit);
        if (auto iit = world_.componentMap_.find(c.index);
            iit != world_.componentMap_.end()) {
          iit->second.pool.Destroy(component);
          iit->second.sparseSet.Remove(c.entity);
        }
      }
    }
  }
  void doDestroyEntity(Entity entity,
                       World::EntityContainer::const_iterator componentIter) {
    for (auto &[id, component] : componentIter->second) {
      auto &componentInfo = world_.componentMap_[id];
      componentInfo.pool.Destroy(component);
      componentInfo.sparseSet.Remove(entity);
    }
    world_.entities_.erase(componentIter);
  }
  void destroyEntity(Entity entity) {
    // Querier querier(world_);
    // if (querier.Has<Node>(entity)) {
    //   auto &node = querier.Get<Node>(entity);
    //   if (node.parent) {
    //     HierarchyRemoveChild(node.parent.value(), entity, querier,
    //                          std::nullopt);
    //   }
    //   destroyEntityTree(entity);
    // } else {
    if (auto componentIter = world_.entities_.find(entity);
        componentIter != world_.entities_.end()) {
      doDestroyEntity(entity, componentIter);
    }
    // }
  }
};