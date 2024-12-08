////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CAMERA_HPP
#define MGL_CAMERA_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace mgl {

class Camera;

///////////////////////////////////////////////////////////////////////// Camera

class Camera {
 private:
  GLuint UboId;
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;

  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f); // Initial position
  glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // Initial orientation
  float radius = 5.0f; // Distance from the origin

  double lastX = 0.0, lastY = 0.0;
  bool dragging = false;

 public:
  explicit Camera(GLuint bindingpoint);
  virtual ~Camera();
  glm::mat4 getViewMatrix() const;
  void setViewMatrix(const glm::mat4 &viewmatrix);
  glm::mat4 getProjectionMatrix() const;
  void setProjectionMatrix(const glm::mat4 &projectionmatrix);

  void updateViewMatrix();

  void onMouseMove(GLFWwindow* window, double xpos, double ypos);
  void onScroll(GLFWwindow* window, double xoffset, double yoffset);
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_CAMERA_HPP */
