#include "simpleGeometryRenderSystem.h"

#include "commonComponents/transform2d.h"
#include "resources/resourceManager.h"
#include "simpleGeometry.h"
#include "util/check.h"
#include "util/fs.hpp"
#include "util/toString.h"
#include "window/windowSize.h"
namespace {
void doRender(SimpleGeometryRenderer* renderer, Transform2D& transform2D,
              GeometryData& geometryData, SimpleGeometry& simpmleGeometry) {
  // std::cout << "position " << toString(transform2D.position_) << "\n";
  // std::cout << "size " << toString(transform2D.size_) << "\n";
  // std::cout << "rotation_ " << transform2D.rotation_ << "\n";
  // std::cout << "color " << toString(simpmleGeometry.color_) << "\n";
  // std::cout << "vao " << geometryData.geometryVAO_ << "\n";
  // std::cout << "index size " << geometryData.indexSize() << "\n";
  SimpleGeometryRendererPrimiteType primitiveType{
      SimpleGeometryRendererPrimiteType::UNKOWN};
  if (simpmleGeometry.type_ == SimpleGeometryType::TRIANGLE) {
    primitiveType = SimpleGeometryRendererPrimiteType::TRIANGLE;
  } else if (simpmleGeometry.type_ == SimpleGeometryType::LINE ||
             simpmleGeometry.type_ == SimpleGeometryType::ARROW) {
    primitiveType = SimpleGeometryRendererPrimiteType::LINE;
  } else {
    CHECK_WITH_INFO(false, "unkown simple geometry type")
  }

  renderer->drawGeometry(primitiveType, geometryData.geometryVAO_,
                         geometryData.indexSize(), transform2D.position_,
                         transform2D.size_, transform2D.rotation_,
                         simpmleGeometry.color_);
}
}  // namespace

void SimpleGeometryRenderSystem::start(World& world) {
  CHECK(fs::exist("./simpleGeometry.vert"))
  CHECK(fs::exist("./simpleGeometry.frag"))
  ResourceManager::loadShader("./simpleGeometry.vert", "./simpleGeometry.frag",
                              nullptr, "simpleGeometry");
  renderer_ = std::make_unique<SimpleGeometryRenderer>(
      ResourceManager::getShader("simpleGeometry"));
  auto windowSize = world.getResource<WindowSize>();
  CHECK(windowSize != nullptr)
  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<float>(windowSize->width_), 0.0f,
                 static_cast<float>(windowSize->height_), -1.0f, 1.0f);
  ResourceManager::getShader("simpleGeometry")
      .use()
      .setMatrix4("projection", projection);
  initData();
}
void SimpleGeometryRenderSystem::render(World& world) {
  auto& transfrom2DEntities = world.getEntities<Transform2D>();
  if (transfrom2DEntities.empty()) return;
  auto& simpleGeometryEntities = world.getEntities<SimpleGeometry>();
  if (simpleGeometryEntities.empty()) return;
  auto& minSizeEntities = min(transfrom2DEntities, simpleGeometryEntities);
  for (auto& entityID : minSizeEntities) {
    auto* transfrom2DComponent = world.getComponent<Transform2D>(entityID);
    auto* simpleGeometry2DComponent =
        world.getComponent<SimpleGeometry>(entityID);
    if (transfrom2DComponent == nullptr ||
        simpleGeometry2DComponent == nullptr) {
      continue;
    }
    auto& geometry = geometryDatas_[size_t(simpleGeometry2DComponent->type_)];
    CHECK(geometry != nullptr)
    doRender(renderer_.get(), *transfrom2DComponent, *geometry,
             *simpleGeometry2DComponent);
  }
}
void SimpleGeometryRenderSystem::initData() {
  geometryDatas_.resize(size_t(SimpleGeometryType::MAX));
  {
    std::vector<float> triangle{
        0.0f,  0.0f,  //
        -1.0f, 1.0f,  //
        -1.0f, -1.0f  //
    };
    geometryDatas_[size_t(SimpleGeometryType::TRIANGLE)] =
        std::make_unique<GeometryData>(triangle);
    std::vector<float> line{
        -1.0f, 0.0f,  //
        0.0f, 0.0f    //
    };
    geometryDatas_[size_t(SimpleGeometryType::LINE)] =
        std::make_unique<GeometryData>(line);
    std::vector<float> arrow{-1.0f, 0.0f,  //
                             0.0f,  0.0f,  //
                             0.0f,  0.0f,  //
                             -0.3f, 0.1f,  //
                             0.0f,  0.0f,  //
                             -0.3f, -0.1f};
    geometryDatas_[size_t(SimpleGeometryType::ARROW)] =
        std::make_unique<GeometryData>(arrow);
  }
}