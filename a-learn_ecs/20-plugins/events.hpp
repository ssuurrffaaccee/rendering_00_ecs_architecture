#pragma once
#include <functional>
#include <vector>
#include <optional>
// one type one event staging
template <typename T> class EventStaging final {
public:
  static void Set(const T &t) { event_ = t; }

  static void Set(T &&t) { event_ = std::move(t); }

  static T &Get() { return *event_; }

  static bool Has() { return event_ != std::nullopt; }

  static void Clear() { event_ = std::nullopt; }

private:
  inline static std::optional<T> event_ = std::nullopt;
};
template <typename T> class EventReader final {
public:
  bool Has() const { return EventStaging<T>::Has(); }

  const T &Read() { return EventStaging<T>::Get(); }

  void Clear() { EventStaging<T>::Clear(); }

  operator bool() const { return Has(); }
};
class Events;
template <typename T> class EventWriter final {
public:
  EventWriter(Events &e) : events_(e) {}

  //! @brief write event data, it can be read after `world.Update()`
  //! @param t the data you want save
  void Write(const T &t);
  //! @brief write event data immediatly(don't delay to `world.Update()`)
  //! @param t the data you want save
  void WriteImmediate(const T &t);
  //! @brief write event data immediatly(don't delay to `world.Update()`)
  //! @param t the data you want save
  void WriteImmediate(T &&t);

private:
  Events &events_;
};
class World;

class Events final {
public:
  friend class World;

  template <typename T> friend class EventWriter;

  template <typename T> auto Reader() {
    removeEventFuncs_.push_back([]() { EventStaging<T>::Clear(); });
    return EventReader<T>{};
  }

  template <typename T> auto Writer() { return EventWriter<T>{*this}; }

// private:
  std::vector<void (*)(void)> removeEventFuncs_;
  std::vector<std::function<void(void)>> addEventFuncs_;

  void addAllEvents() {
    for (auto func : addEventFuncs_) {
      func();
    }
    addEventFuncs_.clear();
  }

  void removeAllEvents() {
    for (auto func : removeEventFuncs_) {
      func();
    }
    removeEventFuncs_.clear();
  }
};

template <typename T> void EventWriter<T>::Write(const T &t) {
  events_.addEventFuncs_.push_back([=]() { EventStaging<T>::Set(t); });
}

template <typename T> void EventWriter<T>::WriteImmediate(const T &t) {
  EventStaging<T>::Set(t);
}

template <typename T> void EventWriter<T>::WriteImmediate(T &&t) {
  EventStaging<T>::Set(std::move(t));
}