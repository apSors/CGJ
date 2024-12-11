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
#include <utility>

namespace mgl {

    ///////////////////////////////////////////////////////////////////////// Camera

    Camera::Camera(GLuint bindingpoint)
        : ViewMatrix(glm::mat4(1.0f)),
        ProjectionMatrix(glm::mat4(1.0f)),
        orientation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))),
        radius(5.0f),
        isPerspective(true),
        left(-2.0f), right(2.0f), bottom(-2.0f), top(2.0f), nearPlane(1.0f), farPlane(10.0f) {
        glGenBuffers(1, &UboId);
        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, 0, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingpoint, UboId);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    Camera::~Camera() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glDeleteBuffers(1, &UboId);
    }

    GLuint Camera::getUboId() const {
        return UboId;
    }

    glm::mat4 Camera::getViewMatrix() const { return ViewMatrix; }

    void Camera::setViewMatrix(const glm::mat4& viewmatrix) {
        ViewMatrix = viewmatrix;

        glm::mat3 rotMat = glm::mat3(ViewMatrix);
        position = glm::vec3(ViewMatrix[3]);
        orientation = glm::quat_cast(rotMat);

        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
            glm::value_ptr(ViewMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    glm::mat4 Camera::getProjectionMatrix() const { return ProjectionMatrix; }

    bool Camera::getIsPerspective() const { return isPerspective; }

    void Camera::setProjectionMatrix(const glm::mat4& projectionmatrix, bool perspective) {
        ProjectionMatrix = projectionmatrix;
        isPerspective = perspective; // Update the projection state
        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
            glm::value_ptr(ProjectionMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void mgl::Camera::onMouseMove(GLFWwindow* window, double xpos, double ypos) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
            dragging = false;
            return;
        }
        if (!dragging) {
            lastX = xpos;
            lastY = ypos;
            dragging = true;
            return;
        }
        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(ypos - lastY);

        lastX = xpos;
        lastY = ypos;

        if (xoffset == 0 && yoffset == 0) {
            return;
        }
        const float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        glm::quat yaw = glm::angleAxis(glm::radians(-xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitch = glm::angleAxis(glm::radians(-yoffset), glm::vec3(1.0f, 0.0f, 0.0f));

        orientation = glm::normalize(yaw * orientation);
        orientation = glm::normalize(orientation * pitch);

        glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
        position = -forward * radius;
        ViewMatrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::mat3_cast(orientation) * glm::vec3(0.0f, 1.0f, 0.0f));

        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(ViewMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Camera::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
        const float zoomSensitivity = 0.05f;
        float delta = -static_cast<float>(yoffset) * zoomSensitivity;
            adjustDistance(delta);
    }

    void Camera::adjustDistance(float delta) {
        radius += delta;
        if (radius < 0.5f) {
            radius = 0.5f;
        }
        glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
        position = -forward * radius;

        ViewMatrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::mat3_cast(orientation) * glm::vec3(0.0f, 1.0f, 0.0f));

        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
            glm::value_ptr(ViewMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    ////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
