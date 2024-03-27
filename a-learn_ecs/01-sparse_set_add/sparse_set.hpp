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
  void Add(T t) {
    density_.push_back(t);
    assure(t);
    index(t) = density_.size() - 1;
  }

private:
  std::vector<T> density_;
  std::vector<std::unique_ptr<std::array<T, PageSize>>>
      pagedSparse_; // maybe use hashmap?????? pageId->Page
  static constexpr T null = std::numeric_limits<T>::max();

  size_t page(T t) const { return t / PageSize; }
  size_t offset(T t) const { return t % PageSize; }

  T index(T t) const { return pagedSparse_[page(t)]->at(offset(t)); }

  T &index(T t) { return pagedSparse_[page(t)]->at(offset(t)); }
  void assure(T t) {
    auto p = page(t);
    if (p >= pagedSparse_.size()) {
      for (size_t i = pagedSparse_.size(); i <= p; i++) {
        pagedSparse_.emplace_back(std::make_unique<std::array<T, PageSize>>());
        pagedSparse_[i]->fill(null);
      }
    }
  }
};