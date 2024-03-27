#pragma once
#include <glm/glm.hpp>
bool intersctionSegmentSegment(const glm::vec2& a, const glm::vec2& b,
                               const glm::vec2& c, const glm::vec2& d,
                               float& rayt);
float rotateRadian(glm::vec2& src, glm::vec2& dst);
inline float radian(glm::vec2& v) { return glm::atan(v[1], v[0]); }

inline glm::vec2 clamp(glm::vec2& v, float maxLen) {
  float len = glm::length(v);
  if (len > maxLen) {
    return glm::normalize(v) * maxLen;
  }
  return v;
}

inline float loopRepeat(float pos, float left, float right) {
  float len = right - left;
  return left + std::fmod(pos - left + len, len);
};

inline float keepRange(float pos, float left, float right) {
  if (pos < left) {
    return right - (left - pos);
  }
  if (pos > right) {
    return left + (pos - right);
  }
  return pos;
}
inline bool inRange(glm::vec2& pos, const glm::vec2& origin,
                    const glm::vec2& shape) {
  auto relativePos = pos - origin;
  if (relativePos[0] <= shape[0] && relativePos[0] >= 0.0f &&
      relativePos[1] <= shape[1] && relativePos[1] >= 0.0f) {
    return true;
  }
  return false;
}
inline float clamp(float vs, float low, float high) {
  if (vs < low) {
    return low;
  }
  if (vs > high) {
    return high;
  }
  return vs;
}

inline float smooth_step(float l, float r, float v) {
  float t = clamp((v - l) / (r - l), 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t);
}