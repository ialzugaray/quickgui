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

//    Eigen::Matrix<Scalar,3,3> view_matrix;
//    view_matrix.topLeftCorner<3,3>() = R.transpose();
//    view_matrix.topRightCorner<3,1>() = -R.transpose() * position;
//    view_matrix.row(3) << 0, 0, 0, 1;

    return  Quaternion{R};

//    Vector3 toVector = (destPoint - sourcePoint).normalized();
//
//    //compute rotation axis
//    Vector3 rotAxis = front.cross(toVector).normalized();
//    if (rotAxis.squaredNorm() == 0) { rotAxis = up; }
//
//    //find the angle around rotation axis
//    Scalar dot = front.dot(toVector);
//    Scalar ang = std::acos(dot);
//
//    //convert axis angle to quaternion
//    return Quaternion{Eigen::AngleAxisd( ang * rotAxis)};
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

#include "impl/gui_impl.hpp"