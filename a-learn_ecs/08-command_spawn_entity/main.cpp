#include <functional>
#include <iostream>

#include "command.hpp"
#include "id.hpp"
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
int main() {
  World world;
  Commands cmd{world};
  /////////////////////////
  Entity entityA = cmd.SpawnImmediateAndReturn(Point0{0, 1}, Point1{1, 2});
  /////////////////////////
  cmd.Spawn(Point0{3, 4}, Point1{5, 6});
  Entity entityB = cmd.SpawnAndReturn(Point0{7, 8}, Point1{9, 10});
  cmd.Execute();
  return 0;
}