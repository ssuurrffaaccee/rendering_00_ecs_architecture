#include "entityInitSystem.h"

#include "commonComponents/transform2d.h"
#include "forceSystem/flowField.hpp"
#include "movementSystem/movement2d.h"
#include "resources/resourceManager.h"
#include "simpleGeometryRenderSystem/simpleGeometry.h"
#include "spriteRenderSystem/spritColor.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/geometry.h"
#include "util/random.h"
#include "window/windowSize.h"
void EntityInitSystem::start(World &world) {
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr) {
    auto flowField = FlowField{};
    flowField.centor_ =
        glm::vec2{windowSize->width_ / 2.0f, windowSize->height_ / 2.0f};
    flowField.step_ = 100;
    flowField.forceMagnitude_ = 100.0f;

    world.addResource<FlowField>(flowField);
    for (float x = 0.5f; x < float(windowSize->width_); x += 50.0f) {
      for (float y = 0.5f; y < float(windowSize->width_); y += 50.0f) {
        auto flow = EntityIDGenerator::alloc();
        auto flowForce = flowField.getCurlForce(glm::vec2{x, y});
        world.addComponent(flow,
                           Transform2D{glm::vec2{x, y}, glm::vec2{50.0f, 50.0f},
                                       radian(flowForce)});
        world.addComponent(flow, SimpleGeometry{SimpleGeometryType::ARROW,
                                                glm::vec3{1.0f, 1.0f, 1.0f}});
      }
    }
  }
  {
    for (int i = 0; i < 1000; i++) {
      auto simpleGeometry = EntityIDGenerator::alloc();
      world.addComponent(
          simpleGeometry,
          Transform2D{glm::vec2{float(windowSize->width_) * myRandom(),
                                float(windowSize->height_) * myRandom()},
                      glm::vec2{50.0f, 50.0f * 0.3f}, 0.0f});
      world.addComponent(
          simpleGeometry,
          SimpleGeometry{SimpleGeometryType::TRIANGLE,
                         glm::vec3{myRandom(), myRandom(), myRandom()}});
      world.addComponent(simpleGeometry,
                         Movement2D{glm::vec2{myRandom(), myRandom()} * 500.0f,
                                    glm::vec2{0.0f, 0.0f}, 100.0f, 500.0f});
    }
  }
  // {
  //   CHECK(fs::exist("../../resources/textures/background.jpg"))
  //   ResourceManager::loadTexture("../../resources/textures/background.jpg",
  //                                false, "background");
  //   auto backgroundEntityID = EntityIDGenerator::alloc();
  //   auto windowSize = world.getResource<WindowSize>();
  //   CHECK(windowSize != nullptr)
  //   world.addComponent(
  //       backgroundEntityID,
  //       Transform2D{glm::vec2{0.0f, 0.0f},
  //                   glm::vec2{windowSize->width_, windowSize->height_}, 0.0});
  //   world.addComponent(backgroundEntityID,
  //                      SpritColor{glm::vec3{1.0f, 1.0f, 1.0f},
  //                                 ResourceManager::getTexture("background")});
  // }
}