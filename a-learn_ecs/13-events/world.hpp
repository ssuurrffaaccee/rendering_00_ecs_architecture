#pragma once
#include <cassert>
#include <functional>
#include <unordered_map>
#include <vector>

#include "id.hpp"
#include "sparse_set.hpp"
#define assertm(msg, expr) assert(((void)msg, (expr)))
class Commands;
class Querier;
class World final {
 public:
  World() = default;
  World(const World &) = delete;
  World &operator=(const World &) = delete;
  friend class Commands;
  friend class Querier;

 private:
  //////////////////////////////////////////////////////////////////////////////////
  struct Pool final {
    std::vector<void *> instances;
    std::vector<void *> cache;

    using CreateFunc = std::function<void *(void)>;
    using DestroyFunc = std::function<void(void *)>;

    CreateFunc create;
    DestroyFunc destroy;

    Pool(CreateFunc create, DestroyFunc destroy)
        : create(create), destroy(destroy) {
      assertm("you must give a non-nullptr create func", create);
      assertm("you must give a non-nullptr destroy func", create);
    }
    ~Pool() {
      for (void *data : cache) {
        destroy(data);
      }
      for (void *data : instances) {
        destroy(data);
      }
    }
    void *Create() {
      if (!cache.empty()) {
        instances.push_back(cache.back());
        cache.pop_back();
      } else {
        instances.push_back(create());
      }
      return instances.back();
    }

    void Destroy(void *elem) {
      if (auto it = std::find(instances.begin(), instances.end(), elem);
          it != instances.end()) {
        cache.push_back(*it);
        std::swap(*it, instances.back());
        instances.pop_back();
      } else {
        assertm("your element not in pool", false);
      }
    }
  };
  struct ComponentInfo {
    Pool pool;
    SparseSets<Entity, 32> sparseSet;

    ComponentInfo(Pool::CreateFunc create, Pool::DestroyFunc destroy)
        : pool(create, destroy) {}

    ComponentInfo() : pool(nullptr, nullptr) {}
  };

  using ComponentTypeMap = std::unordered_map<ComponentTypeID, ComponentInfo>;
  ComponentTypeMap componentMap_;
  //////////////////////////////////////////////////////////////////////////////////
  using ComponentContainer = std::unordered_map<ComponentTypeID, void *>;
  using EntityContainer = std::unordered_map<Entity, ComponentContainer>;
  EntityContainer entities_;
  //////////////////////////////////////////////////////////////////////////////////
};