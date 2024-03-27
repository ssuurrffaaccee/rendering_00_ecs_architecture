#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "util/check.h"

struct MovementSystem : public System {
  MovementSystem() {}
  virtual ~MovementSystem() {}
  void start(World& world) override;
  void update(float dt, World& world) override;

 private:
  float windowWidth_{800.0f};
  float windowHeight_{600.0f};
};