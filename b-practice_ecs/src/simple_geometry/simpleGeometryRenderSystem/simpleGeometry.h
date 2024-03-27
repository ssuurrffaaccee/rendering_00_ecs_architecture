#pragma once
#include <glm/glm.hpp>
enum class SimpleGeometryType { UNKNOWN, TRIANGLE, LINE, ARROW, MAX };
struct SimpleGeometry {
  SimpleGeometryType type_;
  glm::vec3 color_;
};