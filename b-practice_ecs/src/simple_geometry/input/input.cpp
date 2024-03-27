#include "input.h"

#include <array>

#include "cursorPos.h"
#include "ecs/world.h"
#include "inputStatus.h"
#include "util/check.h"
#include "window/window.h"

unsigned int k_complement_control_command = 0xFFFFFFFF;

void Input::onKey(int key, int scancode, int action, int mods) {
  m_game_command &=
      (k_complement_control_command ^ (unsigned int)GameCommand::jump);

  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        // close();
        break;
      case GLFW_KEY_R:
        break;
      case GLFW_KEY_A:
        m_game_command |= (unsigned int)GameCommand::left;
        break;
      case GLFW_KEY_S:
        m_game_command |= (unsigned int)GameCommand::backward;
        break;
      case GLFW_KEY_W:
        m_game_command |= (unsigned int)GameCommand::forward;
        break;
      case GLFW_KEY_D:
        m_game_command |= (unsigned int)GameCommand::right;
        break;
      case GLFW_KEY_SPACE:
        m_game_command |= (unsigned int)GameCommand::jump;
        break;
      case GLFW_KEY_LEFT_CONTROL:
        m_game_command |= (unsigned int)GameCommand::squat;
        break;
      case GLFW_KEY_LEFT_ALT: {
        window_->setFocusMode(!window_->getFocusMode());
      } break;
      case GLFW_KEY_LEFT_SHIFT:
        m_game_command |= (unsigned int)GameCommand::sprint;
        break;
      case GLFW_KEY_F:
        m_game_command ^= (unsigned int)GameCommand::free_carema;
        break;
      default:
        break;
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        // close();
        break;
      case GLFW_KEY_R:
        break;
      case GLFW_KEY_W:
        m_game_command &=
            (k_complement_control_command ^ (unsigned int)GameCommand::forward);
        break;
      case GLFW_KEY_S:
        m_game_command &= (k_complement_control_command ^
                           (unsigned int)GameCommand::backward);
        break;
      case GLFW_KEY_A:
        m_game_command &=
            (k_complement_control_command ^ (unsigned int)GameCommand::left);
        break;
      case GLFW_KEY_D:
        m_game_command &=
            (k_complement_control_command ^ (unsigned int)GameCommand::right);
        break;
      case GLFW_KEY_LEFT_CONTROL:
        m_game_command &=
            (k_complement_control_command ^ (unsigned int)GameCommand::squat);
        break;
      case GLFW_KEY_LEFT_SHIFT:
        m_game_command &=
            (k_complement_control_command ^ (unsigned int)GameCommand::sprint);
        break;
      default:
        break;
    }
  }
}

void Input::onCursorPos(double current_cursor_x, double current_cursor_y) {
  if (window_->getFocusMode()) {
    cursor_delta_x_ = last_cursor_x_ - current_cursor_x;
    cursor_delta_y_ = last_cursor_y_ - current_cursor_y;
  }
  last_cursor_x_ = current_cursor_x;
  last_cursor_y_ = current_cursor_y;
  auto* cursorPos = world_->getResource<CursorPos>();
  cursorPos->position_ =
      glm::vec2{float(last_cursor_x_), float(last_cursor_y_)};
}

void Input::clear() {
  cursor_delta_x_ = 0;
  cursor_delta_y_ = 0;
}

void Input::initialize(Window* window, World* world) {
  window_ = window;
  CHECK(window_ != nullptr);
  world_ = world;
  CHECK(world != nullptr);
  world_->addResource(InputStatus{k_complement_control_command});
  world->addResource(CursorPos{glm::vec2{0.0f, 0.0f}});
  window_->registerOnKeyFunc(
      [this](int key, int scancode, int action, int mods) {
        this->onKey(key, scancode, action, mods);
      });
  window_->registerOnCursorPosFunc(
      [this](double current_cursor_x, double current_cursor_y) {
        this->onCursorPos(current_cursor_x, current_cursor_y);
      });
}

void Input::update() {
  clear();
  if (window_->getFocusMode()) {
    m_game_command &=
        (k_complement_control_command ^ (unsigned int)GameCommand::invalid);
  } else {
    m_game_command |= (unsigned int)GameCommand::invalid;
  }
  InputStatus* inputStatus = world_->getResource<InputStatus>();
  inputStatus->gameCommand_ = m_game_command;
}
