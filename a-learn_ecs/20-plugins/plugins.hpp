#pragma once
class World;
class Plugins {
 public:
  virtual ~Plugins() = default;

  virtual void Build(World *world) = 0;
  virtual void Quit(World *world) = 0;
};