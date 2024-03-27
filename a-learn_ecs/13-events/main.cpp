#include <functional>
#include <iostream>

#include "command.hpp"
#include "events.hpp"
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
//
int main() {
  Events e;
  e.Writer<Point1>().Write(Point1{1, 2});
  e.addAllEvents();
  if (e.Reader<Point1>().Has()) {
    auto point1 = e.Reader<Point1>().Read();
    std::cout << point1.x << " " << point1.y << "\n";
  }
  e.addAllEvents();
  e.removeAllEvents();
  return 0;
}