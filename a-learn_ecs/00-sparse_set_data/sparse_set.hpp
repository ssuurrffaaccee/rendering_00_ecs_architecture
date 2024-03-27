#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

template <typename T, size_t PageSize,
          typename = std::enable_if<std::is_integral_v<T>>>
class SparseSets final {
public:
private:
  std::vector<T> density_;                                       // data index
  std::vector<std::unique_ptr<std::array<T, PageSize>>> sparse_; // index
  static constexpr T null = std::numeric_limits<T>::max();
};