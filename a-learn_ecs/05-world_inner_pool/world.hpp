#pragma once
#include "id.hpp"
#include <functional>
#include <cassert>
#include <unordered_map>
#include <vector>
#define assertm(msg, expr) assert(((void)msg, (expr)))
class World final {
public:
  World() = default;
  World(const World &) = delete;
  World &operator=(const World &) = delete;

  // private:
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
};