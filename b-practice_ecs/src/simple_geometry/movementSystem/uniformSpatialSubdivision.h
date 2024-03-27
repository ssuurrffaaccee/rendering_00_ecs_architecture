#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "ecs/id.h"
#include "util/check.h"
#include "util/toString.h"
static std::vector<std::pair<int, int>> NEIGHBORS{{-1, 1},  {0, 1},  {1, 1},
                                                  {-1, 0},  {0, 0},  {1, 0},
                                                  {-1, -1}, {0, -1}, {1, -1}};
class UniformSpatialSubdivision {
 public:
  int width_{800};
  int height_{600};

  int step_{10};
  void init() {
    col_ = width_ / step_ + 1;
    row_ = height_ / step_ + 1;
    hashedSpace_.resize(col_ * row_);
  }
  void remove(glm::vec2& pos, EntityID entity) {
    auto& entitySet = get(pos);
    entitySet.erase(entity);
  }
  void add(glm::vec2& pos, EntityID entity) {
    auto& entitySet = get(pos);
    entitySet.insert(entity);
  }
  std::vector<EntityID> getNeighbors(glm::vec2& pos) {
    std::vector<EntityID> entities;
    auto [x, y] = hash(pos);
    for (auto [dx, dy] : NEIGHBORS) {
      auto neighborX = x + dx;
      auto neighborY = y + dy;
      if (neighborX >= 0 && neighborX < col_ && neighborY >= 0 &&
          neighborY < row_) {
        auto& curNeighbors = get(neighborX, neighborY);
        entities.insert(entities.end(), curNeighbors.begin(),
                        curNeighbors.end());
      }
    }
    return entities;
  }
  std::vector<std::unordered_set<EntityID>> hashedSpace_;

 private:
  std::unordered_set<EntityID>& get(glm::vec2& pos) {
    int x = int((pos[0] - 1) / step_);
    int y = int((pos[1] - 1) / step_);
    return hashedSpace_[y * col_ + x];
  }
  std::unordered_set<EntityID>& get(int x, int y) {
    return hashedSpace_[y * col_ + x];
  }
  std::pair<int, int> hash(glm::vec2& pos) {
    int x = int(pos[0]) / step_;
    int y = int(pos[1]) / step_;
    return std::make_pair(x, y);
  }
  int col_{0};
  int row_{0};
};