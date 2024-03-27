#pragma once
#include "id.hpp"
#include "query_condition.hpp"
#include "world.hpp"

class Querier final {
 public:
  Querier(World &world) : world_(world) {}
  template <typename T>
  T &Get(Entity entity) {
    auto index = IndexGetter::Get<T>();
    return *((T *)world_.entities_[entity][index]);
  }
  bool Alive(Entity entity) {
    return world_.entities_.find(entity) != world_.entities_.end();
  }
  template <typename T>
  bool Has(Entity entity) const {
    return queryCondition<T>(entity);
  }

 private:
  World &world_;
  template <typename T>
  bool queryCondition(Entity entity) const {
    if constexpr (IsConditionV<T>) {
      using extractor = ConditionExtractor<T>;
      return doQueryCondition<0, typename extractor::args>(entity,
                                                           extractor::type);
    } else {
      return queryExists<T>(entity);
    }
  }

  template <size_t Idx, typename TupleT>
  bool doQueryCondition(Entity entity, ConditionType type) const {
    if constexpr (Idx < std::tuple_size_v<TupleT>) {
      bool result = queryConditionElem<Idx, TupleT>(entity);
      switch (type) {
        case ConditionType::With:
          if (!result) {
            return false;
          } else {
            return doQueryCondition<Idx + 1, TupleT>(entity, type);
          }
        case ConditionType::Option:
          if (result) {
            return true;
          } else {
            return doQueryCondition<Idx + 1, TupleT>(entity, type);
          }
        case ConditionType::Without:
          if (result) {
            return false;
          } else {
            return doQueryCondition<Idx + 1, TupleT>(entity, type);
          }
      }
      return false;
    }
    return true;
  }

  template <size_t Idx, typename TupleT>
  bool queryConditionElem(Entity entity) const {
    using T = std::tuple_element_t<Idx, TupleT>;
    if constexpr (IsConditionV<T>) {
      return queryCondition<T>(entity);
    } else {
      return queryExists<T>(entity);
    }
  }

  template <typename T>
  bool queryExists(Entity entity) const {
    auto index = IndexGetter::Get<T>();
    auto &componentContainer = world_.entities_[entity];
    return componentContainer.find(index) != componentContainer.end();
  }
};