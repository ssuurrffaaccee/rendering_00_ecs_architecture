#include <functional>
#include <iostream>

#include "id.hpp"
#include "sparse_set.hpp"
#include "world.hpp"
struct Point {
  float x;
  float y;
};
int main() {
  World world;
  ComponentTypeID pointTypeID = IndexGetter::Get<Point>();
  world.componentMap_.insert(
      {pointTypeID,
       World::ComponentInfo{[]() { return new Point{}; },
                            [](void *data) { delete (Point *)data; }}});
  Entity entity = EntityGenerator::Gen();
  void *point = world.componentMap_[pointTypeID].pool.create();
  world.componentMap_[pointTypeID].sparseSet.Add(entity);
  return 0;
}