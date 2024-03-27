#pragma once
#include "id.hpp"
#include "world.hpp"

class Querier final {
 public:
  Querier(World &world) : world_(world) {}
  template <typename T>
  T &Get(Entity entity) {
    auto index = IndexGetter::Get<T>();
    return *((T *)world_.entities_[entity][index]);
  }
  bool Alive(Entity entity) {
    return world_.entities_.find(entity) != world_.entities_.end();
  }

 private:
  World &world_;
};