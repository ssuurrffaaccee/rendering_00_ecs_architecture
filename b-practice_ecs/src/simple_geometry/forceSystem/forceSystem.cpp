#include "forceSystem.h"

#include "commonComponents/transform2d.h"
#include "flowField.hpp"
#include "input/cursorPos.h"
#include "movementSystem/movement2d.h"
#include "movementSystem/uniformSpatialSubdivision.h"
#include "util/geometry.h"
#include "util/toString.h"
#include "window/windowSize.h"
namespace {
void addForce(World &world, float dt, Transform2D &transform,
              Movement2D &movement, float windowWidth, float windowHeight,
              float cursorPosX, float cursorPosY, FlowField &flowField,
              UniformSpatialSubdivision &uniformSpatialSubdivision) {
  auto curPos = glm::vec2{cursorPosX, windowHeight - cursorPosY};  // glsl
  if (!inRange(curPos, glm::vec2{0.0f, 0.0f},
               glm::vec2{windowWidth, windowHeight})) {
    return;
  }
  glm::vec2 newAcceleration{0.0f, 0.0f};
  {  // to cursor
    auto newA = (curPos - transform.position_);
    auto d = glm::length(newA);
    auto direction = glm::normalize(newA);
    if (d > movement.arrivalDistance_) {
      newA = direction * movement.maxAcceleration_;
    } else {
      float magnitude = smooth_step(0, movement.arrivalDistance_, d) *
                        movement.maxAcceleration_;
      newA = direction * magnitude;
    }
    newAcceleration += newA - movement.velocity_;
    newAcceleration = clamp(newAcceleration, 10.0f);
  }
  {  // follow flowfield
    auto flowFieldForce = flowField.getCurlForce(transform.position_);
    newAcceleration += flowFieldForce;
    newAcceleration = clamp(newAcceleration, movement.maxAcceleration_);
  }
  {
    auto neighbors =
        uniformSpatialSubdivision.getNeighbors(transform.position_);
    for (auto neighbor : neighbors) {
      auto *transfrom2DComponent = world.getComponent<Transform2D>(neighbor);
      auto *movement2DComponent = world.getComponent<Movement2D>(neighbor);
      if (transfrom2DComponent == nullptr || movement2DComponent == nullptr) {
        continue;
      }
      auto seperationForce =
          transform.position_ - transfrom2DComponent->position_;
      auto d = glm::length(seperationForce);
      if (d < 1e-5) {
        continue;
      }
      seperationForce = glm::normalize(seperationForce) * (1.0f / d);
      newAcceleration += seperationForce;
    }
    newAcceleration = clamp(newAcceleration, movement.maxAcceleration_);
  }
  movement.acceleration_ = clamp(newAcceleration, movement.maxAcceleration_);
}
}  // namespace
void ForceSystem::start(World &world) {
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  windowWidth_ = windowSize->width_;
  windowHeight_ = windowSize->height_;
}
void ForceSystem::update(float dt, World &world) {
  auto &transfrom2DEntities = world.getEntities<Transform2D>();
  if (transfrom2DEntities.empty()) return;
  auto &movment2DEntities = world.getEntities<Movement2D>();
  if (movment2DEntities.empty()) return;
  auto &minSizeEntities = min(transfrom2DEntities, movment2DEntities);
  auto cursorPos = world.getResource<CursorPos>();
  CHECK(cursorPos != nullptr)
  auto flowField = world.getResource<FlowField>();
  CHECK(flowField != nullptr)
  auto uniformSpatialSubdivision =
      world.getResource<std::shared_ptr<UniformSpatialSubdivision>>();
  CHECK(uniformSpatialSubdivision != nullptr)
  for (auto &entityID : minSizeEntities) {
    auto *transfrom2DComponent = world.getComponent<Transform2D>(entityID);
    auto *movement2DComponent = world.getComponent<Movement2D>(entityID);
    if (transfrom2DComponent == nullptr || movement2DComponent == nullptr) {
      continue;
    }
    addForce(world, dt, *transfrom2DComponent, *movement2DComponent,
             windowWidth_, windowHeight_, cursorPos->position_[0],
             cursorPos->position_[1], *flowField,
             *(*uniformSpatialSubdivision).get());
  }
}