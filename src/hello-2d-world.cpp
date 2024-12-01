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
#include <vector>


////////////////////////////////////////////////////////////////////////// OBJECTS

class DrawableObject {
public:
    virtual ~DrawableObject() = default;
    virtual void draw(const std::unique_ptr<mgl::ShaderProgram>& shaders, GLint matrixId, const glm::mat4& transform, const glm::vec4& color) = 0;
};


class Parallelogram : public DrawableObject {
private:
    GLuint vaoId;

public:
    Parallelogram(GLuint vao) : vaoId(vao) {}

    void draw(const std::unique_ptr<mgl::ShaderProgram>& shaders, GLint matrixId, const glm::mat4& transform, const glm::vec4& color) override {
        glBindVertexArray(vaoId);
        shaders->bind();
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(shaders->Uniforms["objectColor"].index, color.r, color.g, color.b, color.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));
        shaders->unbind();
        glBindVertexArray(0);
    }
};

class Square : public DrawableObject {
private:
    GLuint vaoId;

public:
    Square(GLuint vao) : vaoId(vao) {}

    void draw(const std::unique_ptr<mgl::ShaderProgram>& shaders, GLint matrixId, const glm::mat4& transform, const glm::vec4& color) override {
        glBindVertexArray(vaoId);
        shaders->bind();
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(shaders->Uniforms["objectColor"].index, color.r, color.g, color.b, color.a);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));
        shaders->unbind();
        glBindVertexArray(0);
    }
};

class RightTriangle : public DrawableObject {
private:
    GLuint vaoId;

public:
    RightTriangle(GLuint vao) : vaoId(vao) {}

    void draw(const std::unique_ptr<mgl::ShaderProgram>& shaders, GLint matrixId, const glm::mat4& transform, const glm::vec4& color) override {
        glBindVertexArray(vaoId);
        shaders->bind();
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform4f(shaders->Uniforms["objectColor"].index, color.r, color.g, color.b, color.a);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(0));
        shaders->unbind();
        glBindVertexArray(0);
    }
};


////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowCloseCallback(GLFWwindow* win) override;
    void windowSizeCallback(GLFWwindow* win, int width, int height) override;

private:
    const GLuint POSITION = 0, COLOR = 1;
    GLuint ParallelogramVaoId, SquareVaoId, RightTriangleVaoId;
    GLuint ParallelogramVboId[2], SquareVboId[2], RightTriangleVboId[2];
    std::unique_ptr<mgl::ShaderProgram> Shaders;
    GLint MatrixId;

    std::unique_ptr<Parallelogram> parallelogram;
    std::unique_ptr<Square> square;
    std::unique_ptr<RightTriangle> rightTriangle;

    void createShaderProgram();
    void createBufferObjects();
    void destroyBufferObjects();
    void drawScene();
};


//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderProgram() {
    Shaders = std::make_unique<mgl::ShaderProgram>();
    Shaders->addShader(GL_VERTEX_SHADER, "clip-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "clip-fs.glsl");

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
    {{ 0.0f,  -1.0f, 0.0f, 1.0f}},
    {{ 1.0f,  -1.0f, 0.0f, 1.0f}},
    {{ 1.0f,  -2.0f, 0.0f, 1.0f}},
};

const GLubyte ParallelogramIndices[] = {
    0, 1, 2,
    2, 1, 3
};

const Vertex SquareVertices[] = {
    {{ 0.0f, 0.0f, 0.0f, 1.0f}},
    {{ 1.0f, 0.0f, 0.0f, 1.0f}},
    {{ 1.0f, 1.0f, 0.0f, 1.0f}},
    {{ 0.0f, 1.0f, 0.0f, 1.0f}},
};

const GLubyte SquareIndices[] = {
    0, 1, 2,
    0, 2, 3
};

const Vertex RightTriangleVertices[] = {
    {{ 0.0f,  0.0f, 0.0f, 1.0f}},
    {{ -1.0f,  1.0f, 0.0f, 1.0f}},
    {{ -1.0f,  0.0f, 0.0f, 1.0f}},
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

const float scaleFactor = 1.0f / 2.5f;

const glm::mat4 offset_center = glm::translate(glm::vec3((-glm::sqrt(2.0f) + 1) / 2.0f * scaleFactor, 0.0f, 0.0f));

// Parallelogram
const glm::mat4 M_parallelogram = offset_center * glm::translate(glm::vec3(-2.0f * scaleFactor, 2.0f * scaleFactor, 0.0f)) *
glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Medium Triangle
const glm::mat4 M_right_triangle_1 = offset_center * glm::translate(glm::vec3(1.0f * scaleFactor, glm::sqrt(2.0f) * scaleFactor, 0.0f)) *
glm::rotate(glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(glm::sqrt(2.0f) * scaleFactor, glm::sqrt(2.0f) * scaleFactor, 1.0f));

// Small Triangle LEFT
const glm::mat4 M_right_triangle_2 = offset_center * glm::translate(glm::vec3(((-2.0f * glm::sqrt(2.0f)) + 1.0f) * scaleFactor, -2.0f * scaleFactor, 0.0f)) *
glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Small Triangle RIGHT
const glm::mat4 M_right_triangle_3 = offset_center * glm::translate(glm::vec3(1.0f * scaleFactor, -2.0f * scaleFactor, 0.0f)) *
glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));

// Large Triangle BOTTOM
const glm::mat4 M_large_triangle_1 = offset_center * glm::translate(glm::vec3(1.0f * scaleFactor, 0.0f, 0.0f)) *
glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(2.0f * scaleFactor, 2.0f * scaleFactor, 1.0f));

// Large Triangle TOP
const glm::mat4 M_large_triangle_2 = offset_center * glm::translate(glm::vec3(1.0f * scaleFactor, 0.0f, 0.0f)) *
glm::rotate(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(2.0f * scaleFactor, 2.0f * scaleFactor, 1.0f));

// Square
const glm::mat4 M_square = offset_center * glm::translate(glm::vec3(1.0f * scaleFactor, 0.0, 0.0f)) *
glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
glm::scale(glm::vec3(1.0f * scaleFactor, 1.0f * scaleFactor, 1.0f));


void MyApp::drawScene() {
    Shaders->bind();

    parallelogram->draw(Shaders, MatrixId, M_parallelogram, glm::vec4(1.0f, 0.3f, 0.3f, 1.0f)); // Red

    rightTriangle->draw(Shaders, MatrixId, M_right_triangle_1, glm::vec4(1.0f, 1.0f, 0.6f, 1.0f)); // Yellow

    rightTriangle->draw(Shaders, MatrixId, M_right_triangle_2, glm::vec4(1.0f, 0.75f, 0.85f, 1.0f)); // Pink

    rightTriangle->draw(Shaders, MatrixId, M_large_triangle_1, glm::vec4(0.6f, 0.7f, 1.0f, 1.0f)); // Blue

    rightTriangle->draw(Shaders, MatrixId, M_large_triangle_2, glm::vec4(0.7f, 0.9f, 0.5f, 1.0f)); // Green

    rightTriangle->draw(Shaders, MatrixId, M_right_triangle_3, glm::vec4(0.85f, 0.6f, 0.4f, 1.0f)); // Orange

    rightTriangle->draw(Shaders, MatrixId, M_right_triangle_2, glm::vec4(1.0f, 0.75f, 0.85f, 1.0f)); // Pink

    rightTriangle->draw(Shaders, MatrixId, M_right_triangle_1, glm::vec4(1.0f, 1.0f, 0.6f, 1.0f)); // Yellow

    square->draw(Shaders, MatrixId, M_square, glm::vec4(0.7f, 0.6f, 1.0f, 1.0f)); // Purple

    Shaders->unbind();
}


////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
    createBufferObjects();
    createShaderProgram();

    parallelogram = std::make_unique<Parallelogram>(ParallelogramVaoId);
    square = std::make_unique<Square>(SquareVaoId);
    rightTriangle = std::make_unique<RightTriangle>(RightTriangleVaoId);
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
    engine.setWindow(600, 600, "2D Tangram", 0, 1);
    engine.init();
    engine.run();
    exit(EXIT_SUCCESS);
}


//////////////////////////////////////////////////////////////////////////// END