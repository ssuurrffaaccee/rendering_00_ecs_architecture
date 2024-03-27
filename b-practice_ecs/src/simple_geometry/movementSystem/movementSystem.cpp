#include "movementSystem.h"

#include <cmath>

#include "commonComponents/transform2d.h"
#include "movement2d.h"
#include "uniformSpatialSubdivision.h"
#include "util/geometry.h"
#include "util/toString.h"
#include "window/windowSize.h"
namespace {
void doMove(EntityID entityID, float dt, Transform2D &transform,
            Movement2D &movement, float windowWidth, float windowHeight,
            UniformSpatialSubdivision &uniformSpatialSubdivision) {
  uniformSpatialSubdivision.remove(transform.position_, entityID);
  auto newV = movement.velocity_ + movement.acceleration_ * dt;
  auto newP = transform.position_ + 0.5f * (newV + movement.velocity_) * dt;
  transform.position_ = glm::vec2{keepRange(newP[0], 0.0f, windowWidth),
                                  keepRange(newP[1], 0.0f, windowHeight)};
  uniformSpatialSubdivision.add(transform.position_, entityID);
  if (glm::dot(newV, newV) > 0.0f) {
    transform.rotation_ = radian(newV);
  }
  movement.velocity_ = clamp(newV, movement.maxSpeed_);
}
}  // namespace
void MovementSystem::start(World &world) {
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  windowWidth_ = windowSize->width_;
  windowHeight_ = windowSize->height_;
  auto uniformSpatialSubdivision =
      std::make_shared<UniformSpatialSubdivision>();
  uniformSpatialSubdivision->height_ = windowHeight_;
  uniformSpatialSubdivision->width_ = windowWidth_;
  uniformSpatialSubdivision->step_ = 400;
  uniformSpatialSubdivision->init();
  world.addResource<std::shared_ptr<UniformSpatialSubdivision>>(
      uniformSpatialSubdivision);
}
void MovementSystem::update(float dt, World &world) {
  auto &transfrom2DEntities = world.getEntities<Transform2D>();
  if (transfrom2DEntities.empty()) return;
  auto &movment2DEntities = world.getEntities<Movement2D>();
  if (movment2DEntities.empty()) return;
  auto &minSizeEntities = min(transfrom2DEntities, movment2DEntities);
  auto uniformSpatialSubdivision =
      world.getResource<std::shared_ptr<UniformSpatialSubdivision>>();
  for (auto &entityID : minSizeEntities) {
    auto *transfrom2DComponent = world.getComponent<Transform2D>(entityID);
    auto *movement2DComponent = world.getComponent<Movement2D>(entityID);
    if (transfrom2DComponent == nullptr || movement2DComponent == nullptr) {
      continue;
    }
    doMove(entityID, dt, *transfrom2DComponent, *movement2DComponent,
           windowWidth_, windowHeight_, *(*uniformSpatialSubdivision).get());
  }
}