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

  glm::vec3 position;
  glm::quat orientation;
  float radius = 5.0f;

  bool isPerspective;
  float left, right, bottom, top, nearPlane, farPlane;

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
  void adjustDistance(float delta);

  glm::vec3 getPosition() const { return position; }
  void setPosition(const glm::vec3& newPosition) { position = newPosition; }

  glm::quat getOrientation() const { return orientation; }
  void setOrientation(const glm::quat& newOrientation) { orientation = newOrientation; }

  void onMouseMove(GLFWwindow* window, double xpos, double ypos);
  void onScroll(GLFWwindow* window, double xoffset, double yoffset);
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_CAMERA_HPP */
