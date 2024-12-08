////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "./mglCamera.hpp"

#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp> 
#include <GLFW/glfw3.h>


namespace mgl {

///////////////////////////////////////////////////////////////////////// Camera

Camera::Camera(GLuint bindingpoint)
    : ViewMatrix(glm::mat4(1.0f)),
    ProjectionMatrix(glm::mat4(1.0f)),
    orientation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))),
    radius(5.0f) {
  glGenBuffers(1, &UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 0, GL_STREAM_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, UboId);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  updateViewMatrix();
}

Camera::~Camera() {
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glDeleteBuffers(1, &UboId);
}

glm::mat4 Camera::getViewMatrix() const { return ViewMatrix; }

void Camera::setViewMatrix(const glm::mat4 &viewmatrix) {
  ViewMatrix = viewmatrix;
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(ViewMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix() const { return ProjectionMatrix; }

void Camera::setProjectionMatrix(const glm::mat4 &projectionmatrix) {
  ProjectionMatrix = projectionmatrix;
  glBindBuffer(GL_UNIFORM_BUFFER, UboId);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                  glm::value_ptr(ProjectionMatrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (firstMouse) {
            // Initialize lastX and lastY correctly based on the current mouse position
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            return;  // Avoid applying movement on the first frame of dragging
        }

        // Calculate mouse movement deltas
        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);

        // Sensitivity to control speed of camera movement
        float sensitivity = 0.005f;  // Lower sensitivity for smoother control

        // Update yaw and pitch
        float yaw = -deltaX * sensitivity;
        float pitch = -deltaY * sensitivity;

        // Rotate the orientation quaternion
        glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));

        // Combine rotations
        orientation = glm::normalize(yawQuat * orientation * pitchQuat);

        // Update last mouse positions
        lastX = xpos;
        lastY = ypos;

        // Recalculate the view matrix
        updateViewMatrix();
    }
    else {
        firstMouse = true;  // Reset when the mouse button is released
    }
}




void Camera::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
    // Adjust radius (zoom level)
    radius -= static_cast<float>(yoffset) * 0.1f;

    // Clamp radius to a reasonable range
    if (radius < 2.0f) radius = 2.0f;
    if (radius > 20.0f) radius = 20.0f;

    // Recalculate the view matrix to reflect new radius
    updateViewMatrix();
}



void Camera::updateViewMatrix() {
    // Calculate the forward direction (camera's negative Z-axis)
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);

    // Calculate the camera's new position based on radius and forward direction
    position = -forward * radius;

    // Calculate the up direction (camera's positive Y-axis)
    glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

    // Update the view matrix to look at the origin (center) with the new position
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    ViewMatrix = glm::lookAt(position, center, up);

    // Send the updated matrix to the GPU
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(ViewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}




////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
