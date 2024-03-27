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

int main() {
  World world;
  world.AddStartupSystem([](Commands &cmd, Resources res) {
    cmd.SpawnImmediateAndReturn(Point0{0, 1});
  });
  world.AddSystem(
      [](Commands &cmd, Querier query, Resources res, Events &events) {
        auto entitys = query.Query<Point0>();
        for (auto e : entitys) {
          auto& com = query.Get<Point0>(e);
          com.x++;
          com.y++;
        }
      });
  world.AddSystem(
      [](Commands &cmd, Querier query, Resources res, Events &events) {
        auto entitys = query.Query<Point0>();
        for (auto e : entitys) {
          auto& com = query.Get<Point0>(e);
          std::cout << com.x << " " << com.y << "\n";
        }
      });
  world.Startup();
  world.Update();
  world.Update();
  world.Update();
  world.Shutdown();
  return 0;
}