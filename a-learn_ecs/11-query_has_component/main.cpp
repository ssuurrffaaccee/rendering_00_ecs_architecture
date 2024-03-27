#include <functional>
#include <iostream>

#include "command.hpp"
#include "id.hpp"
#include "query.hpp"
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
  Commands cmd_0{world};
  /////////////////////////
  Entity entityA = cmd_0.SpawnImmediateAndReturn(Point0{0, 1}, Point1{1, 2});
  /////////////////////////
  cmd_0.Spawn(Point0{3, 4}, Point1{5, 6});
  Entity entityB = cmd_0.SpawnAndReturn(Point0{7, 8}, Point1{9, 10});
  cmd_0.Execute();
  /////////////////////////
  Commands cmd_1{world};
  cmd_1.DestroyComponent<Point0>(entityA).AddComponent(entityA, Point0{11, 12});
  cmd_1.DestroyEntity(entityB);
  cmd_1.Execute();
  ////////////////////////
  Querier query{world};
  assert(query.Alive(entityB) == false);
  Point0& point0OfA = query.Get<Point0>(entityA);
  std::cout << point0OfA.x << " " << point0OfA.y << "\n";
  ///////////////////////////////////
  auto condition0 = query.Has<Option<Point0>>(entityA);
  assert(condition0 == true);
  auto condition1 = query.Has<With<Point0, Point1>>(entityA);
  assert(condition1 == true);
  auto condition2 = query.Has<Without<Point0>>(entityA);
  assert(condition2 == false);
  ////////////////////////////////////////////////
  Commands cmd_2{world};
  cmd_2.Spawn(Point0{13, 14});
  cmd_2.Execute();
  ///////////////////////////////////////////////////
  auto entities_0 = query.Query<Point0>();
  assert(entities_0.size()==2);
  auto entities_1 = query.Query<Point1>();
  assert(entities_1.size()==1);
  auto entities_2 = query.Query<Without<Point1>>();
  assert(entities_2.size()==1);
  return 0;
}