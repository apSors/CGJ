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

void Camera::setViewMatrix(const glm::mat4& viewmatrix) {
    ViewMatrix = viewmatrix;
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
        glm::value_ptr(ViewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix() const { return ProjectionMatrix; }

void Camera::setProjectionMatrix(const glm::mat4& projectionmatrix) {
    ProjectionMatrix = projectionmatrix;
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
        glm::value_ptr(ProjectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void mgl::Camera::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    // Check if the left mouse button is pressed
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        dragging = false; // Reset dragging state if button is released
        return;
    }

    if (!dragging) {
        // Initialize the last mouse position when the movement starts
        lastX = xpos;
        lastY = ypos;
        dragging = true;
        return;
    }

    // Calculate the offset from the last mouse position only after the first drag
    float xoffset = static_cast<float>(xpos - lastX);
    float yoffset = static_cast<float>(lastY - ypos); // Inverted for natural movement

    // Update the last mouse position
    lastX = xpos;
    lastY = ypos;

    // Avoid applying any offsets until the first actual drag is made
    if (xoffset == 0 && yoffset == 0) {
        return; // Skip movement if there’s no actual mouse offset
    }

    // Apply sensitivity to control movement speed
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Create rotation quaternions for pitch (vertical) and yaw (horizontal)
    glm::quat pitch = glm::angleAxis(glm::radians(-yoffset), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X
    glm::quat yaw = glm::angleAxis(glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));   // Rotate around Y

    // Combine the rotations and update the orientation
    orientation = glm::normalize(yaw * orientation * pitch);

    // Update the view matrix to reflect the new orientation
    updateViewMatrix();
}





void Camera::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
    // Define zoom sensitivity
    const float zoomSensitivity = 0.1f;

    // Adjust the radius based on the scroll offset
    radius += static_cast<float>(yoffset) * zoomSensitivity;

    // Ensure that the radius doesn't go below a minimum value (e.g., 1.0f)
    if (radius < 0.5f) {
        radius = 0.5f;
    }

    // Ensure that the radius doesn't exceed a maximum value (optional)
    if (radius > 100.0f) {
        radius = 100.0f;
    }

    // Recalculate the camera's position based on the updated radius
    glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
    position = -forward * radius;

    // Update the view matrix to reflect the new zoom level
    updateViewMatrix();
}





void Camera::updateViewMatrix() {
    // Calculate the forward direction (camera's negative Z-axis) using the quaternion
    glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, 5.0f);

    // Calculate the camera's position based on the forward direction and radius
    position = forward * radius;

    // Calculate the up direction (camera's positive Y-axis)
    glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);

    // The center point is usually the origin for camera look-at purposes
    glm::vec3 center(0.0f, 0.0f, 0.0f);

    // Create the view matrix using glm::lookAt
    ViewMatrix = glm::lookAt(position, center, up);

    // Send the updated view matrix to the GPU
    glBindBuffer(GL_UNIFORM_BUFFER, UboId);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(ViewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Camera::adjustDistance(float delta) {
    // Adjust the radius (distance from the center)
    radius += delta;

    // Ensure that the radius doesn't go below 1.0f
    if (radius < 1.0f) {
        radius = 1.0f;
    }

    // Calculate the new position based on the orientation and the new radius
    glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
    position = -forward * radius;

    // Update the view matrix based on the new position
    updateViewMatrix();
}


////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
