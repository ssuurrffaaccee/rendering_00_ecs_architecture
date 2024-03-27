#include "sparse_set.hpp"
#include <iostream>
int main() {
  SparseSets<int32_t, 64> sparserSet;
  sparserSet.Add(7);
  sparserSet.Add(77);
  sparserSet.Add(777);
  sparserSet.Remove(77);
  assert(sparserSet.Contain(77) == false);
  for (auto &index : sparserSet) {
    std::cout << index << "\n";
  }
  sparserSet.Clear();
  return 0;
}