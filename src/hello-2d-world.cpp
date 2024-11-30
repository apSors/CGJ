////////////////////////////////////////////////////////////////////////////////
//
// Drawing two instances of a triangle in Clip Space.
// A "Hello 2D World" of Modern OpenGL.
//
// Copyright (c) 2013-24 by Carlos Martinho
//
// INTRODUCES:
// GL PIPELINE, mglShader.hpp, mglConventions.hpp
//
////////////////////////////////////////////////////////////////////////////////

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

#include "../mgl/mgl.hpp"


////////////////////////////////////////////////////////////////////////// MYAPP


////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowCloseCallback(GLFWwindow* win) override;
    void windowSizeCallback(GLFWwindow* win, int width, int height) override;

private:
    const GLuint POSITION = 0, COLOR = 1;
    GLuint VaoId, VboId[2];
    GLuint ParallelogramVaoId, SquareVaoId, RightTriangleVaoId;
    GLuint ParallelogramVboId[2], SquareVboId[2], RightTriangleVboId[2];
    std::unique_ptr<mgl::ShaderProgram> Shaders;
    GLint MatrixId;

    void createShaderProgram();
    void createBufferObjects();
    void destroyBufferObjects();
    void drawScene();
};

//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderProgram() {
    Shaders = std::make_unique<mgl::ShaderProgram>();
    Shaders->addShader(GL_VERTEX_SHADER, "shaders/clip-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "shaders/clip-fs.glsl");

    Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, POSITION);
    Shaders->addAttribute(mgl::COLOR_ATTRIBUTE, COLOR);
    Shaders->addUniform("Matrix");
    Shaders->addUniform("objectColor"); // Add uniform for object color

    Shaders->create();

    MatrixId = Shaders->Uniforms["Matrix"].index;
    // Add uniform location for the object color
    GLuint colorId = Shaders->Uniforms["objectColor"].index;
}


//////////////////////////////////////////////////////////////////// VAOs & VBOs

typedef struct {
    GLfloat XYZW[4];
} Vertex;

const Vertex ParallelogramVertices[] = {
    {{ 0.0f,  0.0f, 0.0f, 1.0f}},
    {{ glm::sqrt(2.0f),  0.0f, 0.0f, 1.0f}},
    {{ -0.707f,  glm::sqrt(2.0f) / 2, 0.0f, 1.0f}},
    {{ glm::sqrt(2.0f) - 0.707f ,  glm::sqrt(2.0f) / 2, 0.0f, 1.0f}}
};

const GLubyte ParallelogramIndices[] = {
    0, 1, 2,
    2, 1, 3
};

const Vertex SquareVertices[] = {
    {{-0.5f, -0.5f, 0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f, 1.0f}}
};

const GLubyte SquareIndices[] = {
    0, 1, 2,
    2, 1, 3
};

const Vertex RightTriangleVertices[] = {
    {{-0.5f, -0.5f, 0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f, 0.0f, 1.0f}}
};

const GLubyte RightTriangleIndices[] = {
    0, 1, 2
};

void MyApp::createBufferObjects() {
    //Parallelogram
    glGenVertexArrays(1, &ParallelogramVaoId);
    glBindVertexArray(ParallelogramVaoId);
    glGenBuffers(2, ParallelogramVboId);

    glBindBuffer(GL_ARRAY_BUFFER, ParallelogramVboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ParallelogramVertices), ParallelogramVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(COLOR);
    glVertexAttribPointer(COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(sizeof(ParallelogramVertices[0].XYZW)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ParallelogramVboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ParallelogramIndices), ParallelogramIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    //Square
    glGenVertexArrays(1, &SquareVaoId);
    glBindVertexArray(SquareVaoId);
    glGenBuffers(2, SquareVboId);

    glBindBuffer(GL_ARRAY_BUFFER, SquareVboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SquareVertices), SquareVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(COLOR);
    glVertexAttribPointer(COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(sizeof(SquareVertices[0].XYZW)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SquareVboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SquareIndices), SquareIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);

    //Triangles
    glGenVertexArrays(1, &RightTriangleVaoId);
    glBindVertexArray(RightTriangleVaoId);
    glGenBuffers(2, RightTriangleVboId);

    glBindBuffer(GL_ARRAY_BUFFER, RightTriangleVboId[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RightTriangleVertices), RightTriangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION);
    glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(COLOR);
    glVertexAttribPointer(COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(sizeof(RightTriangleVertices[0].XYZW)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RightTriangleVboId[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RightTriangleIndices), RightTriangleIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void MyApp::destroyBufferObjects() {
    glBindVertexArray(ParallelogramVaoId);
    glDisableVertexAttribArray(POSITION);
    glDisableVertexAttribArray(COLOR);
    glDeleteVertexArrays(1, &ParallelogramVaoId);

    glBindVertexArray(SquareVaoId);
    glDisableVertexAttribArray(POSITION);
    glDisableVertexAttribArray(COLOR);
    glDeleteVertexArrays(1, &SquareVaoId);

    glBindVertexArray(RightTriangleVaoId);
    glDisableVertexAttribArray(POSITION);
    glDisableVertexAttribArray(COLOR);
    glDeleteVertexArrays(1, &RightTriangleVaoId);
}

////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 I(1.0f);

const float scaleFactor = 0.4f;

// Parallelogram
const glm::mat4 M_parallelogram = glm::translate(glm::vec3(-0.8815f, 0.395f, 0.0f)) *
glm::rotate(glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Medium Triangle
const glm::mat4 M_right_triangle_1 = glm::translate(glm::vec3(0.5975f, 0.2825f, 0.0f)) *
glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(glm::sqrt(2.0f) * scaleFactor, glm::sqrt(2.0f) * scaleFactor, 1.0f));

// Small Triangle LEFT
const glm::mat4 M_right_triangle_2 = glm::translate(glm::vec3(-0.6250f, -0.5900f, 0.0f)) *
glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Small Triangle RIGHT
const glm::mat4 M_right_triangle_3 = glm::translate(glm::vec3(0.1215f, -0.5935f, 0.0f)) *
glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Large Triangle BOTTOM
const glm::mat4 M_large_triangle_1 = glm::translate(glm::vec3(-0.2500f, 0.0f, 0.0f)) *
glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(2.0f * scaleFactor, 2.0f * scaleFactor, 1.0f));

// Large Triangle TOP
const glm::mat4 M_large_triangle_2 = glm::translate(glm::vec3(-0.0850f, 0.4f, 0.0f)) *
glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(2.0f * scaleFactor, 2.0f * scaleFactor, 1.0f));

// Square
const glm::mat4 M_square = glm::translate(glm::vec3(0.3150f, 0.2825f, 0.0f)) *
glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));


void MyApp::drawScene() {
    // Parallelogram 
    glBindVertexArray(ParallelogramVaoId);
    Shaders->bind();
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_parallelogram));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 1.0f, 0.3f, 0.3f, 1.0f); // Red
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Square 
    glBindVertexArray(SquareVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_square));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 0.7f, 0.6f, 1.0f, 1.0f); //Purple
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Medium Triangle
    glBindVertexArray(RightTriangleVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_right_triangle_1));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 1.0f, 1.0f, 0.6f, 1.0f); //Yellow
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Small Triangle LEFT 
    glBindVertexArray(RightTriangleVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_right_triangle_2));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 1.0f, 0.75f, 0.85f, 1.0f); //Pink
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Small Triangle RIGHT 
    glBindVertexArray(RightTriangleVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_right_triangle_3));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 0.85f, 0.6f, 0.4f, 1.0); //Orange
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Large Triangle SIDE
    glBindVertexArray(RightTriangleVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_large_triangle_1));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 0.6f, 0.7f, 1.0f, 1.0f); //Blue
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    // Large Triangle TOP 
    glBindVertexArray(RightTriangleVaoId);
    glUniformMatrix4fv(MatrixId, 1, GL_FALSE, glm::value_ptr(M_large_triangle_2));
    glUniform4f(Shaders->Uniforms["objectColor"].index, 0.7f, 0.9f, 0.5f, 1.0f); //Green
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));

    Shaders->unbind();
    glBindVertexArray(0);
}





////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
    createBufferObjects();
    createShaderProgram();
}

void MyApp::windowCloseCallback(GLFWwindow* win) { destroyBufferObjects(); }

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
    glViewport(0, 0, winx, winy);
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) { drawScene(); }

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
    mgl::Engine& engine = mgl::Engine::getInstance();
    engine.setApp(new MyApp());
    engine.setOpenGL(4, 6);
    engine.setWindow(600, 600, "Hello Modern 2D World", 0, 1);
    engine.init();
    engine.run();
    exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////// END