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
  Commands cmd_0{world};
  /////////////////////////
  Entity entityA = cmd_0.SpawnImmediateAndReturn(Point0{0, 1}, Point1{1, 2});
  Entity entityB = cmd_0.SpawnImmediateAndReturn(Point0{3, 4}, Point1{5, 6});
  cmd_0.Execute();
  Commands cmd_1{world};
  auto changeHierachy = cmd_1.ChangeHierarchy(entityA);
  changeHierachy.Append(std::vector<Entity>{entityB});
  changeHierachy.Remove(entityB, 0);
  cmd_1.Execute();
  world.Shutdown();
  return 0;
}