#pragma once

enum class ConditionType {
  With,
  Option,
  Without,
};

//! @brief query condition, all conditions must be true
//! @tparam ...Args  conditions, can be component or query condition
//! @see Option Without
template <typename... Args>
struct With {};  // it is all true

//! @brief query condition, any conditions be true will satisfy
//! @tparam ...Args  conditions, can be component or query condition
//! @see With Without
template <typename... Args>
struct Option {};  // it is any true

//! @brief query condition, all conditions be false will satisfy
//! @tparam ...Args  conditions, can be component or query condition
//! @see With Option
template <typename... Args>
struct Without {};  // is is all false

//! @brief query condition extractor, will extract condition arguments and
//! condition type
//! @tparam query condition
//! @see With Option Without
template <typename T>
struct ConditionExtractor;

template <typename... Args>
struct ConditionExtractor<Without<Args...>> {
  using args = std::tuple<Args...>;
  static constexpr ConditionType type = ConditionType::Without;
};

template <typename... Args>
struct ConditionExtractor<With<Args...>> {
  using args = std::tuple<Args...>;
  static constexpr ConditionType type = ConditionType::With;
};

template <typename... Args>
struct ConditionExtractor<Option<Args...>> {
  using args = std::tuple<Args...>;
  static constexpr ConditionType type = ConditionType::Option;
};

template <typename T>
struct IsCondition {
  static constexpr bool value = false;
};

template <typename... Args>
struct IsCondition<With<Args...>> {
  static constexpr bool value = true;
};

template <typename... Args>
struct IsCondition<Option<Args...>> {
  static constexpr bool value = true;
};

template <typename... Args>
struct IsCondition<Without<Args...>> {
  static constexpr bool value = true;
};

//! @brief judge if template T is a query condition
//! @tparam T
//! @see Without With Option
template <typename T>
constexpr auto IsConditionV = IsCondition<T>::value;
