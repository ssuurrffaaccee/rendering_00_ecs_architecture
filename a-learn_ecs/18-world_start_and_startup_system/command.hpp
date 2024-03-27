#pragma once
#include "hierarchy_changer.hpp"
#include "system.hpp"
#include "world.hpp"
#include "resouces.hpp"
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
  template <typename T>
  Commands &SetResource(T &&resource) {
    auto index = IndexGetter::Get<T>();
    if (auto it = world_.resources_.find(index);
        it != world_.resources_.end()) {
      assertm("resource already exists", it->second.resource);
      it->second.resource = new T(std::move(std::forward<T>(resource)));
    } else {
      auto newIt = world_.resources_.insert_or_assign(
          index, World::ResourceInfo([](void *elem) { delete (T *)elem; }));
      newIt.first->second.resource =
          new T(std::move(std::forward<T>(resource)));
    }

    return *this;
  }

  template <typename T>
  Commands &RemoveResource() {
    auto index = IndexGetter::Get<T>();
    destroyResources_.push_back(
        ResourceDestroyInfo(index, [](void *elem) { delete (T *)elem; }));

    return *this;
  }
  HierarchyChanger &ChangeHierarchy(Entity entity) {
    hieChangers_.emplace_back(entity, world_);
    return hieChangers_.back();
  }
  void Execute() {
    for (const auto &info : destroyResources_) {
      removeResource(info);
    }
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
    for (auto &hieChanger : hieChangers_) {
      hieChanger.execute(*this);
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
    Querier querier(world_);
    if (querier.Has<Node>(entity)) {
      auto &node = querier.Get<Node>(entity);
      if (node.parent) {
        HierarchyRemoveChild(node.parent.value(), entity, querier,
                             std::nullopt);
      }
      destroyEntityTree(entity);
    } else {
      if (auto componentIter = world_.entities_.find(entity);
          componentIter != world_.entities_.end()) {
        doDestroyEntity(entity, componentIter);
      }
    }
  }
  void destroyEntityTree(Entity entity) {
    Querier querier(world_);
    if (auto it = world_.entities_.find(entity); it != world_.entities_.end()) {
      auto &node = querier.Get<Node>(entity);
      for (auto child : node.children) {
        destroyEntityTree(child);
      }
      doDestroyEntity(entity, it);
    }
  }
  ///////////////////////////////////////////////////
  using DestroyFunc = void (*)(void *);

  struct ResourceDestroyInfo {
    uint32_t index;
    DestroyFunc destroy;

    ResourceDestroyInfo(uint32_t index, DestroyFunc destroy)
        : index(index), destroy(destroy) {}
  };
  std::vector<ResourceDestroyInfo> destroyResources_;
  void removeResource(const ResourceDestroyInfo &info) {
    if (auto it = world_.resources_.find(info.index);
        it != world_.resources_.end()) {
      info.destroy(it->second.resource);
      it->second.resource = nullptr;
    }
  }
  //////////////////////////////////////////////////
  std::vector<HierarchyChanger> hieChangers_;
};

template <typename T>
World &World::SetResource(T &&resource) {
  Commands commands(*this);
  commands.SetResource(std::forward<T>(resource));

  return *this;
}

inline void World::Startup() {
  // for (auto &plugins : pluginsList_) {
  //     plugins->Build(this);
  // }
  for (auto sys : startupSystems_) {
    Commands commands{*this};
    sys(commands, Resources{*this});
    commands.Execute();
  }
}