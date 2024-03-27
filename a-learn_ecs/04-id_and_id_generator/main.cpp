#include "id.hpp"
#include <functional>
#include "sparse_set.hpp"
#include <iostream>
struct Point {
  float x;
  float y;
};
int main() {
  ComponentTypeID pointTypeID = IndexGetter::Get<Point>();
  std::cout << pointTypeID << "\n";
  std::cout << EntityGenerator::Gen() << "\n";
  return 0;
}