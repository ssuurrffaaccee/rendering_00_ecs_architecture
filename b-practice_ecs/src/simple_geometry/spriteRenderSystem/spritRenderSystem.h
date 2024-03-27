#pragma once
#include "util/check.h"

#include "ecs/system.h"
#include "ecs/world.h"
#include "renderers/spriteRenderer.h"

struct SpritRenderSystem : public System {
  SpritRenderSystem() {}
  virtual ~SpritRenderSystem() {}
  void start(World& world) override;
  void render(World& world) override;

 private:
  std::unique_ptr<SpriteRenderer> renderer_{nullptr};
};