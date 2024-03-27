#include "id.hpp"
#include <functional>
#include "sparse_set.hpp"
#include "world.hpp"
#include <iostream>
struct Point {
  float x;
  float y;
};
int main() {
  World world;
  World::Pool pool{[]() { return new Point{}; },
                   [](void *data) { delete (Point *)data; }};
  void *point = pool.Create();
  pool.Destroy(point);
  return 0;
}