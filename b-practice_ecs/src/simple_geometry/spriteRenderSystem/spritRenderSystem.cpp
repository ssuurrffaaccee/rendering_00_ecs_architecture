#include "spritRenderSystem.h"

#include "commonComponents/transform2d.h"
#include "resources/resourceManager.h"
#include "spritColor.h"
#include "util/fs.hpp"
#include "util/toString.h"
#include "window/windowSize.h"
void doRender(SpriteRenderer* renderer, Transform2D& transform2D,
              SpritColor& spritColor) {
  // std::cout << "texture ID " << spritColor.spritTexture_.ID << "\n";
  // std::cout << "position " << toString(transform2D.position_) << "\n";
  // std::cout << "size " << toString(transform2D.size_) << "\n";
  // std::cout << "rotation_ " << transform2D.rotation_ << "\n";
  // std::cout << "color " << toString(spritColor.baseColor_) << "\n";
  renderer->drawSprite(spritColor.spritTexture_, transform2D.position_,
                       transform2D.size_, transform2D.rotation_,
                       spritColor.baseColor_);
}
void SpritRenderSystem::start(World& world) {
  CHECK(fs::exist("./sprite.vert"))
  CHECK(fs::exist("./sprite.frag"))
  ResourceManager::loadShader("./sprite.vert", "./sprite.frag", nullptr,
                              "sprite");
  renderer_ =
      std::make_unique<SpriteRenderer>(ResourceManager::getShader("sprite"));
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  // std::cout << "window size:" << windowSize->width_ << " "
  //           << windowSize->height_ << "\n";
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(windowSize->width_), 0.0f,
                 static_cast<float>(windowSize->height_), -1.0f, 1.0f);
  ResourceManager::getShader("sprite").use().setMatrix4("projection",
                                                        projection);
  ResourceManager::getShader("sprite").setInteger("sprite", 0);
}
void SpritRenderSystem::render(World& world) {
  auto& transfrom2DEntities = world.getEntities<Transform2D>();
  if (transfrom2DEntities.empty()) return;
  auto& SpritColorEntities = world.getEntities<SpritColor>();
  if (SpritColorEntities.empty()) return;
  auto& minSizeEntities = min(transfrom2DEntities, transfrom2DEntities);
  for (auto& entityID : minSizeEntities) {
    auto* transfrom2DComponent = world.getComponent<Transform2D>(entityID);
    auto* SpritColor2DComponent = world.getComponent<SpritColor>(entityID);
    if (transfrom2DComponent == nullptr || SpritColor2DComponent == nullptr) {
      continue;
    }
    doRender(renderer_.get(), *transfrom2DComponent, *SpritColor2DComponent);
  }
}
