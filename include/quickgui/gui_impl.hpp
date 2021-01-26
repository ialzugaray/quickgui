// Copyright (c) 2021 by Ignacio Alzugaray <alzugaray dot ign at gmail dot com>
// ETH Zurich, Vision for Robotics Lab.

namespace quickgui {

auto Gui::DrawCube() -> void {
  glBegin(GL_QUADS);// Begin drawing the color cube with 6 quads
  // Top face (y = 1.0f)
  // Define vertices in counter-clockwise (CCW) order with normal pointing out
  glColor3f(0.0f, 1.0f, 0.0f);// Green
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);

  // Bottom face (y = -1.0f)
  glColor3f(1.0f, 0.5f, 0.0f);// Orange
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);

  // Front face  (z = 1.0f)
  glColor3f(1.0f, 0.0f, 0.0f);// Red
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);

  // Back face (z = -1.0f)
  glColor3f(1.0f, 1.0f, 0.0f);// Yellow
  glVertex3f(1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, -1.0f);

  // Left face (x = -1.0f)
  glColor3f(0.0f, 0.0f, 1.0f);// Blue
  glVertex3f(-1.0f, 1.0f, 1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);

  // Right face (x = 1.0f)
  glColor3f(1.0f, 0.0f, 1.0f);// Magenta
  glVertex3f(1.0f, 1.0f, -1.0f);
  glVertex3f(1.0f, 1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, 1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glEnd();// End of drawing color-cube
}

Gui::Gui() {
  //  InitializeEnvironment();
  ResetCamera();
}

Gui::~Gui() { DestroyEnvironment(); }

auto Gui::StartInNewThread() -> void {
  //  thread_ = std::thread(&Gui::Run_, this);
  thread_ = std::thread([&]() {
    InitializeEnvironment();
    Run_();
  });

  DLOG(INFO) << "Gui " << thread_.get_id() << " started by thread " << std::this_thread::get_id() << ".";
}

auto Gui::Start() -> void {
  InitializeEnvironment();
  Run_();
}

auto Gui::Run_() -> void {
  glEnable(GL_DEPTH_TEST);

  ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

  // Main loop
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    static bool show_demo_window = true;
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    // Rendering
    ImGui::Render();

    HandleInput();

    LoadProjection();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (1) { DrawCube(); }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
  }
}

auto Gui::InitializeEnvironment() -> void {
  // glfw Initialization
  glfwSetErrorCallback([](int error, const char* description) {
    LOG(ERROR) << "Glfw Error " << error << ": " << std::string(description);
  });

  if (!glfwInit()) LOG(ERROR) << "Glfw could not be initialized";

  // OpenGL + GLSL version: GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+
  // only

  // Create window with graphics context
  std::string window_name = "Window";
  window_ = glfwCreateWindow(1280, 720, window_name.c_str(), NULL, NULL);
  if (window_ == NULL) LOG(ERROR) << "Window could not be created";

  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);// Enable vsync

  //  glfwSetCursorPosCallback(window_, mouse_callback);
  //  glfwSetMouseButtonCallback(window_, MouseButtonCallback);

  // Initialize OpenGL loader
  bool err = glewInit() != GLEW_OK;
  if (err) { LOG(ERROR) << "Failed to initialize OpenGL loader!\n"; }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad
  // Controls
  io.WantCaptureKeyboard = true;
  io.WantCaptureMouse = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

auto Gui::DestroyEnvironment() -> void {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

auto Gui::LoadProjection() -> void {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  int display_w, display_h;
  glfwGetFramebufferSize(window_, &display_w, &display_h);// TODO: Repeated

  gluPerspective(kViewFov_, float(display_w) / display_h, kMinClipDistance_, kMaxClipDistance_);

  glPushMatrix();

  Vector3 camera_front = camera_quat_.toRotationMatrix() * Vector3::UnitZ();
  Vector3 camera_up = Vector3::UnitY();

  gluLookAt(camera_pos_.x(), camera_pos_.y(), camera_pos_.z(), (camera_pos_ + camera_front).x(),
            (camera_pos_ + camera_front).y(), (camera_pos_ + camera_front).z(), camera_up.x(), camera_up.y(),
            camera_up.z());

}

auto Gui::HandleInput() -> void {
  Vector3 camera_z = camera_quat_.toRotationMatrix() * Vector3::UnitZ();
    Vector3 camera_up = Vector3::UnitY();
    Vector3 camera_x = camera_up.cross(camera_z);
    Vector3 camera_y = camera_x.cross(camera_z);
//  Vector3 camera_x = camera_quat_.toRotationMatrix() * Vector3::UnitX();
//  Vector3 camera_y = camera_quat_.toRotationMatrix() * Vector3::UnitY();

  auto frame_dt = 1.0 / ImGui::GetIO().Framerate;
  auto disp = kNavigationTranslationVelocity_ * frame_dt;

  if (ImGui::IsKeyDown(GLFW_KEY_W)) { TranslateCamera(camera_z * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_S)) { TranslateCamera(-camera_z * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_A)) { TranslateCamera(camera_x * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_D)) { TranslateCamera(-camera_x * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_Q)) { TranslateCamera(Vector3::UnitY() * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_E)) { TranslateCamera(-Vector3::UnitY() * disp); }
  if (ImGui::IsKeyDown(GLFW_KEY_R)) { ResetCamera(); }

  static bool dragging_flag = false;


//  camera_quat_ = lookAt(camera_pos_, Vector3::Zero(), Vector3::UnitY());

  if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)
      && ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
    static Eigen::Quaterniond root_quat;

    if (!dragging_flag) {
      dragging_flag = true;
      root_quat = camera_quat_;
    }

    auto delta = ImGui::GetMouseDragDelta();
    auto ang_disp_scale = kNavigationRotationVelocity_;

    camera_quat_ =  Eigen::AngleAxis<Scalar>(-delta.x * ang_disp_scale, Vector3::UnitY()) * root_quat
        * Eigen::AngleAxis<Scalar>(delta.y * ang_disp_scale, Vector3::UnitX())

        ;
  } else {
    dragging_flag = false;
  }
}

auto Gui::ResetCamera() -> void {
  camera_pos_.setZero();
  camera_quat_.setIdentity();
}

auto Gui::TranslateCamera(const Vector3& dt) -> void { camera_pos_ = camera_pos_ + dt; };

}// namespace quickgui