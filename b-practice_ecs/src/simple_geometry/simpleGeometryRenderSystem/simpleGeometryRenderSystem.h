#pragma once
#include "ecs/system.h"
#include "ecs/world.h"
#include "geometryData.h"
#include "renderers/simpleGeometryRenderer.h"
#include "util/check.h"

struct SimpleGeometryRenderSystem : public System {
  SimpleGeometryRenderSystem() {}
  virtual ~SimpleGeometryRenderSystem() {}
  void start(World& world) override;
  void render(World& world) override;

 private:
  void initData();
  std::vector<std::unique_ptr<GeometryData>> geometryDatas_;
  std::unique_ptr<SimpleGeometryRenderer> renderer_{nullptr};
};