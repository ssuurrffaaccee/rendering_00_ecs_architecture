#include <optional>

#include "query.hpp"
#include "world.hpp"
//! @brief ECS Component, identify a node entity in herarchy
//! @note maybe you think component shouldn't in ecs.hpp,
//!        but for supporting herarchy in ecs, we must put it here(for
//!        HierarchyUpdateSystem)
struct Node final {
  std::optional<Entity>
      parent;  //!< parent node, std::nullopt means this node is root
  std::vector<Entity> children;
};
// help functions for operator hierarchy

inline void HierarchyRemoveChild(Entity parent, Entity child, Querier querier,
                                 std::optional<size_t> idx) {
  auto& parentNode = querier.Get<Node>(parent);
  auto& childNode = querier.Get<Node>(child);
  childNode.parent = std::nullopt;
  if (idx) {
    parentNode.children.erase(parentNode.children.begin() + idx.value());
  } else {
    if (auto it = std::find(parentNode.children.begin(),
                            parentNode.children.end(), child);
        it != parentNode.children.end()) {
      parentNode.children.erase(it);
    }
  }
}

inline void HierarchyShiftChild(Entity parent, Entity child, Querier querier,
                                std::optional<size_t> idx) {
  auto& parentNode = querier.Get<Node>(parent);
  auto& childNode = querier.Get<Node>(child);
  if (childNode.parent) {
    HierarchyRemoveChild(childNode.parent.value(), child, querier,
                         std::nullopt);
  }
  childNode.parent = parent;
  if (idx) {
    parentNode.children.insert(parentNode.children.begin() + idx.value(),
                               child);
  } else {
    parentNode.children.push_back(child);
  }
}
class HierarchyChanger final {
 public:
  friend class Commands;

  explicit HierarchyChanger(Entity target, World& world)
      : world_(world), target_(target) {}

  //! @brief remove entity from the target
  //! @param entity
  //! @param idx a hint index, when not std::nullopt, it will remove the entity
  //! at index directly, otherwise will find the entity in target children
  auto& Remove(Entity entity, std::optional<size_t> idx) {
    cmds_.push_back(Cmd{Cmd::Type::Remove, entity, idx});
    return *this;
  }

  //! @brief shift entity to the new target(will auto remove from old parent)
  //! @param entity
  //! @param idx a hint index, when not std::nullopt, it will push back to
  //! target's children, otherwise will insert to the pointed index
  auto& Shift(Entity entity, std::optional<size_t> idx = std::nullopt) {
    cmds_.push_back(Cmd{Cmd::Type::Shift, entity, idx});
    return *this;
  }

  //! @brief appen entities to the target at the end
  //! @param entity
  auto& Append(const std::vector<Entity>& entities) {
    for (auto entity : entities) {
      cmds_.push_back(Cmd{Cmd::Type::Shift, entity, std::nullopt});
    }
    return *this;
  }

 private:
  struct Cmd {
    enum class Type {
      Shift,
      Remove,
    } type;
    Entity entity;
    std::optional<size_t> idxHint;
  };

  World& world_;
  Entity target_;
  std::vector<Cmd> cmds_;

  void execute(Commands& cmds) {
    Querier querier(world_);
    //make sure target aliveing and has children
    if (!querier.Alive(target_) || !querier.Has<Node>(target_)) {
      return;
    }

    for (const auto& cmd : cmds_) {
      if (!querier.Alive(cmd.entity) || !querier.Has<Node>(cmd.entity)) {
        continue;
      }

      if (cmd.type == Cmd::Type::Shift) {
        //shift entity to target' children
        HierarchyShiftChild(target_, cmd.entity, querier, cmd.idxHint);
      } else {
        //remove entity from target' children
        HierarchyRemoveChild(target_, cmd.entity, querier, cmd.idxHint);
      }
    }
  }
};