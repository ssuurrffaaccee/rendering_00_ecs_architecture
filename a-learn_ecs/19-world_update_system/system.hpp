#pragma once
#include <variant>
#include "id.hpp"
class Commands;
class Resources;
class Querier;
class Events;
using StartupSystem = void (*)(Commands &, Resources);

using EachElemUpdateSystem = void (*)(Commands &, Querier, Resources, Events &);
using HierarchyUpdateSystem = void (*)(std::optional<Entity>, Entity,
                                       Commands &, Querier, Resources,
                                       Events &);

using UpdateSystem = std::variant<EachElemUpdateSystem, HierarchyUpdateSystem>;