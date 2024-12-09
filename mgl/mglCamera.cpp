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
        radius(10.0f),
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

    void Camera::setProjectionMatrix(const glm::mat4& projectionmatrix) {
        ProjectionMatrix = projectionmatrix;
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

        if (isPerspective) { // Perspective projection
            radius -= static_cast<float>(yoffset) * zoomSensitivity;
            if (radius < 0.5f) {
                radius = 0.5f;
            }
            if (radius > 100.0f) {
                radius = 100.0f;
            }

            glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
            position = -forward * radius;

            ViewMatrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::mat3_cast(orientation) * glm::vec3(0.0f, 1.0f, 0.0f));

            glBindBuffer(GL_UNIFORM_BUFFER, UboId);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                glm::value_ptr(ViewMatrix));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        else {  // Orthographic projection (FIX)
            float zoomFactor = 1.0f + static_cast<float>(yoffset) * zoomSensitivity;
            left *= zoomFactor;
            right *= zoomFactor;
            bottom *= zoomFactor;
            top *= zoomFactor;

            if (right <= left) {
                right = left + 0.01f;
            }
            if (top <= bottom) {
                top = bottom + 0.01f;
            }
            if (right - left < 0.1f) {
                right = left + 0.1f;
                top = bottom + 0.1f;
            }
            ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);

            glBindBuffer(GL_UNIFORM_BUFFER, UboId);
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(ProjectionMatrix));
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    }

    void Camera::adjustDistance(float delta) {
        radius += delta;
        if (radius < 1.0f) {
            radius = 1.0f;
        }
        glm::vec3 forward = glm::mat3_cast(orientation) * glm::vec3(0.0f, 0.0f, -1.0f);
        position = -forward * radius;

        ViewMatrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glBindBuffer(GL_UNIFORM_BUFFER, UboId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
            glm::value_ptr(ViewMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    ////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl
