#pragma once
#include "command.hpp"
#include "world.hpp"
class Resources final {
public:
  Resources(World &world) : world_(world) {}

  template <typename T> bool Has() const {
    auto index = IndexGetter::Get<T>();
    auto it = world_.resources_.find(index);
    return it != world_.resources_.end() && it->second.resource;
  }

  template <typename T> T &Get() {
    auto index = IndexGetter::Get<T>();
    return *((T *)world_.resources_.at(index).resource);
  }

private:
  World &world_;
};

template <typename T> T *World::GetResource() {
  Resources resources(*this);
  if (!resources.Has<T>()) {
    return nullptr;
  } else {
    return &resources.Get<T>();
  }
}