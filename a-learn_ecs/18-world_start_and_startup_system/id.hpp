#pragma once
#include <stdint.h>
#include <type_traits>
/////////////////////////////////////////////////
using ComponentTypeID = uint32_t;
class IndexGetter final {
public:
    template <typename T>
    static uint32_t Get() {
        static uint32_t id = curIdx_++;
        return id;
    }

private:
    inline static uint32_t curIdx_ = 0;
};
/////////////////////////////////////////////////
using Entity = uint32_t;
template <typename T, typename = std::enable_if<std::is_integral_v<T>>>
struct IDGenerator final {
public:
    static T Gen() { return curId_++; }

private:
    inline static T curId_ = {};
};
using EntityGenerator = IDGenerator<Entity>;
/////////////////////////////////////////////////