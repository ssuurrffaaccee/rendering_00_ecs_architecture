#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>

struct FlowField {
  glm::vec2 getPerlinForce(const glm::vec2& pos) {
    float noise = glm::perlin(pos);
    return glm::vec2{cos(noise), sin(noise)} * forceMagnitude_;
  }
  glm::vec2 getCurlForce(const glm::vec2& pos) {
    auto dir = pos - centor_;
    glm::mat4 rotation{1.0};
    rotation =
        glm::rotate(rotation, 3.1415926f * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
    auto newDir = rotation * glm::vec4{dir[0], dir[1], 0.0f, 1.0f};
    return glm::vec2{newDir};
  }
  float forceMagnitude_{100};
  float step_{10};
  glm::vec2 centor_{500.0f, 500.0f};
};