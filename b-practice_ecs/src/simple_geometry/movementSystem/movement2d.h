#pragma once
#include <glm/glm.hpp>
struct Movement2D {
  glm::vec2 velocity_{0.0f, 0.0f};
  glm::vec2 acceleration_{0.0f, 0.0f};
  float maxSpeed_{1000.0f};
  float maxAcceleration_{500.0f};
  float arrivalDistance_{200.0f};
};