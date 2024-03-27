#include <functional>
#include <iostream>

#include "command.hpp"
#include "events.hpp"
#include "id.hpp"
#include "query.hpp"
#include "resouces.hpp"
#include "sparse_set.hpp"
#include "world.hpp"
struct Point0 {
  float x;
  float y;
};
struct Point1 {
  float x;
  float y;
};
//
int main() {
  World world;
  world.SetResource(Point0{1, 2});
  Commands commamnd{world};
  commamnd.SetResource(Point1{3, 4});
  commamnd.Execute();
  auto *point0ptr = world.GetResource<Point0>();
  assert(point0ptr != nullptr);
  std::cout << point0ptr->x << " " << point0ptr->y << "\n";
  Resources resources{world};
  bool has0 = resources.Has<Point0>();
  assert(has0 == true);
  bool has1 = resources.Has<Point1>();
  assert(has1 == true);
  Point0 &point0 = resources.Get<Point0>();
  Point1 &point1 = resources.Get<Point1>();
  std::cout << point0.x << " " << point0.y << "\n";
  std::cout << point1.x << " " << point1.y << "\n";
  world.Shutdown();
  return 0;
}