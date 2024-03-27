#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>
class GeometryData {
 public:
  GeometryData(const std::vector<float>& data);
  void initData();
  uint32_t indexSize() { return data_.size() / 2; }
  std::vector<float> data_;
  unsigned geometryVAO_;
};