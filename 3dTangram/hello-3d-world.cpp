////////////////////////////////////////////////////////////////////////////////
//
// Adding a camera and moving to the 3rd dimension.
// A "Hello 3D World" of Modern OpenGL.
//
// Copyright (c) 2013-24 by Carlos Martinho
//
// INTRODUCES:
// VIEW PIPELINE, UNIFORM BUFFER OBJECTS, mglCamera.hpp
//
////////////////////////////////////////////////////////////////////////////////

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>
#include <memory>

#include "../mgl/mgl.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/quaternion.hpp>


////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowCloseCallback(GLFWwindow* win) override;
    void windowSizeCallback(GLFWwindow* win, int width, int height) override;
    void cursorCallback(GLFWwindow* win, double xpos, double ypos);
    void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);

private:
    const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;
    GLuint VaoId;

    std::unique_ptr<mgl::ShaderProgram> Shaders = nullptr;
    std::unique_ptr<mgl::Camera> Camera1 = nullptr;
    std::unique_ptr<mgl::Camera> Camera2 = nullptr;
    GLint ModelMatrixId;

    void createShaderProgram();
    void createBufferObjects();
    void destroyBufferObjects();
    void drawScene(double elapsed);

    glm::vec3 targetPoint = glm::vec3(0.0f, 0.0f, 0.0f);  // The point cameras orbit around
    float orbitRadius = 5.0f;  // Radius of the orbit
    float orbitSpeed = 1.0f;   // Speed of the orbit

    glm::quat camera1Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Quaternion for Camera1
    glm::quat camera2Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Quaternion for Camera2

    bool useCamera1 = true;  // Track the active camera

    double lastX = 0.0, lastY = 0.0; // Store the last mouse position for movement
};

////////////////////////////////////////////////////////////////// VAO, VBO, EBO

typedef struct {
  GLfloat XYZW[4];
  GLfloat RGBA[4];
} Vertex;

const Vertex Vertices[] = {
    {{0.0f, 0.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.1f, 1.0f}}, // 0 - FRONT
    {{1.0f, 0.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.1f, 1.0f}}, // 1
    {{1.0f, 1.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.1f, 1.0f}}, // 2
                                                          // 2
    {{0.0f, 1.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.1f, 1.0f}}, // 3
                                                          // 0

    {{1.0f, 0.0f, 1.0f, 1.0f}, {0.1f, 0.9f, 0.1f, 1.0f}}, // 1 - RIGHT
    {{1.0f, 0.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.1f, 1.0f}}, // 5
    {{1.0f, 1.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.1f, 1.0f}}, // 6
                                                          // 6
    {{1.0f, 1.0f, 1.0f, 1.0f}, {0.1f, 0.9f, 0.1f, 1.0f}}, // 2
                                                          // 1

    {{1.0f, 1.0f, 1.0f, 1.0f}, {0.1f, 0.1f, 0.9f, 1.0f}}, // 2 - TOP
    {{1.0f, 1.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.9f, 1.0f}}, // 6
    {{0.0f, 1.0f, 0.0f, 1.0f}, {0.1f, 0.1f, 0.9f, 1.0f}}, // 7
                                                          // 7
    {{0.0f, 1.0f, 1.0f, 1.0f}, {0.1f, 0.1f, 0.9f, 1.0f}}, // 3
                                                          // 2

    {{1.0f, 0.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.9f, 1.0f}}, // 5 - BACK
    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.9f, 1.0f}}, // 4
    {{0.0f, 1.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.9f, 1.0f}}, // 7
                                                          // 7
    {{1.0f, 1.0f, 0.0f, 1.0f}, {0.1f, 0.9f, 0.9f, 1.0f}}, // 6
                                                          // 5

    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.9f, 0.1f, 0.9f, 1.0f}}, // 4 - LEFT
    {{0.0f, 0.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.9f, 1.0f}}, // 0
    {{0.0f, 1.0f, 1.0f, 1.0f}, {0.9f, 0.1f, 0.9f, 1.0f}}, // 3
                                                          // 3
    {{0.0f, 1.0f, 0.0f, 1.0f}, {0.9f, 0.1f, 0.9f, 1.0f}}, // 7
                                                          // 4

    {{0.0f, 0.0f, 1.0f, 1.0f}, {0.9f, 0.9f, 0.1f, 1.0f}}, // 0 - BOTTOM
    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.9f, 0.9f, 0.1f, 1.0f}}, // 4
    {{1.0f, 0.0f, 0.0f, 1.0f}, {0.9f, 0.9f, 0.1f, 1.0f}}, // 5
                                                          // 5
    {{1.0f, 0.0f, 1.0f, 1.0f}, {0.9f, 0.9f, 0.1f, 1.0f}}  // 1
                                                          // 0
};

const unsigned int Indices[] = {
    0,  1,  2,  2,  3,  0,  // FRONT
    4,  5,  6,  6,  7,  4,  // RIGHT
    8,  9,  10, 10, 11, 8,  // TOP
    12, 13, 14, 14, 15, 12, // BACK
    16, 17, 18, 18, 19, 16, // LEFT
    20, 21, 22, 22, 23, 20  // BOTTOM
};

void MyApp::createBufferObjects() {
  GLuint boId[2];

  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);
  {
    glGenBuffers(2, boId);
    glBindBuffer(GL_ARRAY_BUFFER, boId[0]);
    {
      glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(POSITION);
      glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                            reinterpret_cast<void *>(0));

      glEnableVertexAttribArray(COLOR);
      glVertexAttribPointer(
          COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
          reinterpret_cast<GLvoid *>(sizeof(Vertices[0].XYZW)));
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices,
                 GL_STATIC_DRAW);
  }
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(2, boId);
}

void MyApp::destroyBufferObjects() {
  glBindVertexArray(VaoId);
  glDisableVertexAttribArray(POSITION);
  glDisableVertexAttribArray(COLOR);
  glDeleteVertexArrays(1, &VaoId);
  glBindVertexArray(0);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderProgram() {
  Shaders = std::make_unique<mgl::ShaderProgram>();
  Shaders->addShader(GL_VERTEX_SHADER, "3dTangram/color-vs.glsl");
  Shaders->addShader(GL_FRAGMENT_SHADER, "3dTangram/color-fs.glsl");

  Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, POSITION);
  Shaders->addAttribute(mgl::COLOR_ATTRIBUTE, COLOR);
  Shaders->addUniform(mgl::MODEL_MATRIX);
  Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);

  Shaders->create();

  ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 ModelMatrix = glm::translate(glm::vec3(-0.5f, -0.5f, -0.5f)) *
                              glm::scale(glm::vec3(2.0f));

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix1 =
    glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

// Eye(-5,-5,-5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix2 =
    glm::lookAt(glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));

// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
const glm::mat4 ProjectionMatrix1 =
    glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 15.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const glm::mat4 ProjectionMatrix2 =
    glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 1.0f, 15.0f);

void MyApp::drawScene(double elapsed) {
    // Update the camera's position based on the quaternion orientations
    glm::vec3 cameraPosition1 = glm::vec3(orbitRadius * cos(glm::radians(30.0f)) * cos(glm::radians(30.0f)),
        orbitRadius * sin(glm::radians(30.0f)),
        orbitRadius * sin(glm::radians(30.0f)) * cos(glm::radians(30.0f)));

    // Rotate the camera based on the quaternion orientation
    glm::mat4 camera1RotationMatrix = glm::mat4_cast(camera1Orientation);
    glm::mat4 viewMatrix1 = glm::lookAt(cameraPosition1, targetPoint, glm::vec3(0.0f, 1.0f, 0.0f)) * camera1RotationMatrix;
    Camera1->setViewMatrix(viewMatrix1);

    // Repeat for Camera2 with its own orientation
    glm::vec3 cameraPosition2 = glm::vec3(orbitRadius * cos(glm::radians(30.0f)) * cos(glm::radians(30.0f + 180.0f)),
        orbitRadius * sin(glm::radians(30.0f)),
        orbitRadius * sin(glm::radians(30.0f)) * cos(glm::radians(30.0f + 180.0f)));

    glm::mat4 camera2RotationMatrix = glm::mat4_cast(camera2Orientation);
    glm::mat4 viewMatrix2 = glm::lookAt(cameraPosition2, targetPoint, glm::vec3(0.0f, 1.0f, 0.0f)) * camera2RotationMatrix;
    Camera2->setViewMatrix(viewMatrix2);

    // Set the projection matrices (unchanged)
    Camera1->setProjectionMatrix(ProjectionMatrix1);
    Camera2->setProjectionMatrix(ProjectionMatrix2);

    // Render scene from Camera1's perspective
    glBindVertexArray(VaoId);
    Shaders->bind();
    glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    Shaders->unbind();

    // Render scene from Camera2's perspective (if needed)
    Shaders->bind();
    glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    Shaders->unbind();
    glBindVertexArray(0);
}


////////////////////////////////////////////////////////////////////// CALLBACKS

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    mgl::Engine::getInstance().getApp()->cursorCallback(window, xpos, ypos);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    mgl::Engine::getInstance().getApp()->scrollCallback(window, xoffset, yoffset);
}

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);

        float sensitivity = 0.01f;

        glm::quat rotationX = glm::angleAxis(-deltaX * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis rotation (left-right)
        glm::quat rotationY = glm::angleAxis(deltaY * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f)); // X-axis rotation (up-down)

        camera1Orientation = glm::normalize(rotationX * camera1Orientation * rotationY);
        camera2Orientation = glm::normalize(rotationX * camera2Orientation * rotationY);
    }

    lastX = xpos;
    lastY = ypos;
}

void MyApp::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    orbitRadius -= static_cast<float>(yoffset) * 0.1f;

    if (orbitRadius < 2.0f) orbitRadius = 2.0f;
    if (orbitRadius > 20.0f) orbitRadius = 20.0f;
}

void MyApp::initCallback(GLFWwindow* win) {
    createBufferObjects();
    createShaderProgram();

    Camera1 = std::make_unique<mgl::Camera>(UBO_BP);
    Camera2 = std::make_unique<mgl::Camera>(UBO_BP);

    glfwSetWindowUserPointer(win, this);

    glfwSetCursorPosCallback(win, cursor_pos_callback);
    glfwSetScrollCallback(win, scroll_callback);
}

void MyApp::windowCloseCallback(GLFWwindow *win) { destroyBufferObjects(); }

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) {
    drawScene(elapsed);
}
/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(800, 600, "Hello Modern 3D World", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
