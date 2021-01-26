// Copyright (c) 2021 by Ignacio Alzugaray <alzugaray dot ign at gmail dot com>
// ETH Zurich, Vision for Robotics Lab.

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glog/logging.h>

#include <Eigen/Geometry>
#include <thread>

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "implot/implot.h"
#include "implot/implot_internal.h"

namespace quickgui {
class Gui {
 public:
  using Thread = std::thread;
  using Scalar = double;// TODO: add traits
  using Vector3 = Eigen::Matrix<Scalar, 3, 1>;
  using Vector2 = Eigen::Matrix<Scalar, 2, 1>;
  using Quaternion = Eigen::Quaternion<Scalar>;
  using WindowPtr = GLFWwindow*;
  using WindowName = std::string;
  using ColorMap = std::vector<std::array<GLfloat, 3>>;

  inline Gui();
  inline ~Gui();

  inline auto Start() -> void;
  inline auto StartInNewThread() -> void;
  inline virtual auto Run_() -> void;

  inline static auto DrawCube() -> void;

  static auto lookAt(const Vector3& position, const Vector3& target, const Vector3& up) -> Quaternion {

    Eigen::Matrix<Scalar,3,3> R;
    R.col(2) = (target - position).normalized();
    R.col(0) = up.cross(R.col(2)).normalized();
    R.col(1) = R.col(2).cross(R.col(0));
    return  Quaternion{R};
  }

 protected:
  static auto RandomColorMap(size_t n = 100) -> ColorMap;

  inline auto LoadProjection() -> void;
  inline auto InitializeEnvironment() -> void;
  inline auto DestroyEnvironment() -> void;
  inline virtual auto HandleInput() -> void;

  inline virtual auto ResetCamera() -> void;

  inline auto TranslateCamera(const Vector3& dt) -> void;
  inline auto RotateCamera(const Quaternion& dq) -> void;

  static constexpr auto kViewFov_ = 60.0;
  static constexpr auto kMinClipDistance_ = 0.01;
  static constexpr auto kMaxClipDistance_ = 5;
  static constexpr auto kNavigationTranslationVelocity_ = 1.0;
  static constexpr auto kNavigationRotationVelocity_ = 0.005;

  Thread thread_;
  Vector3 camera_pos_;
  Quaternion camera_quat_;
  WindowPtr window_;
};
}// namespace quickgui

#include "gui_impl.hpp"