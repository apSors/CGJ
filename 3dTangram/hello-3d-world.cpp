////////////////////////////////////////////////////////////////////////////////
//
//  Loading meshes from external files
//
// Copyright (c) 2023-24 by Carlos Martinho
//
// INTRODUCES:
// MODEL DATA, ASSIMP, mglMesh.hpp
//
////////////////////////////////////////////////////////////////////////////////

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "../mgl/mgl.hpp"

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowSizeCallback(GLFWwindow* win, int width, int height) override;
    void cursorCallback(GLFWwindow* win, double xpos, double ypos);
    void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);

private:
    const GLuint UBO_BP = 0;
    mgl::ShaderProgram* Shaders = nullptr;
    mgl::Camera* Camera = nullptr;
    GLint ModelMatrixId;
    std::vector<glm::mat4> ModelMatrices;
    std::vector<mgl::Mesh*> Meshes;

    void createMeshes();
    void createShaderPrograms();
    void createCamera();
    void drawScene();
};

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_dir = "assets/";

    std::vector<std::string> mesh_files = {
        "medium_triangle.obj",
        "large_triangle_top.obj",
        "large_triangle_bottom.obj",
        "square.obj",
        "small_triangle_left.obj",
        "small_triangle_right.obj",
        "parallelogram.obj"
    };

    for (size_t i = 0; i < mesh_files.size(); ++i) {
        std::string mesh_fullname = mesh_dir + mesh_files[i];

        mgl::Mesh* mesh = new mgl::Mesh();
        mesh->joinIdenticalVertices();
        mesh->create(mesh_fullname);

        Meshes.push_back(mesh);

        glm::mat4 transformation = glm::mat4(1.0f);

        // transformation = glm::translate(transformation, glm::vec3(1.0f, 1.0f, 1.0f));  // future transformations

        ModelMatrices.push_back(transformation);
    }
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    // Create and compile the shader program
    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, "3dTangram/cube-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "3dTangram/cube-fs.glsl");

    // Add attributes based on the first mesh as a reference
    if (!Meshes.empty()) {
        Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);

        if (Meshes[0]->hasNormals()) {
            Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
        }
        if (Meshes[0]->hasTexcoords()) {
            Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
        }
        if (Meshes[0]->hasTangentsAndBitangents()) {
            Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
        }
    }

    // Add uniform variables for the model matrix, base color, and camera block
    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniform("baseColor"); // Add uniform for base color
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);

    // Create the shader program
    Shaders->create();

    // Retrieve the uniform location for the model matrix
    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}


///////////////////////////////////////////////////////////////////////// CAMERA

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
glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const glm::mat4 ProjectionMatrix2 =
glm::perspective(glm::radians(100.0f), 640.0f / 480.0f, 1.0f, 50.0f);

void MyApp::createCamera() {
    Camera = new mgl::Camera(UBO_BP);
    Camera->setProjectionMatrix(ProjectionMatrix2);
    Camera->setViewMatrix(ViewMatrix2);
    Camera->adjustDistance(-5.0f);
}
/////////////////////////////////////////////////////////////////////////// DRAW

glm::mat4 ModelMatrix(1.0f);

void MyApp::drawScene() {
    Shaders->bind();

    // Define base colors for each mesh
    std::vector<glm::vec3> baseColors = {
        glm::vec3(0.7f, 0.3f, 0.2f), // Red
        glm::vec3(0.4f, 0.7f, 0.3f), // Green
        glm::vec3(0.2f, 0.3f, 0.8f), // Blue
        glm::vec3(0.7f, 0.8f, 0.0f), // Yellow
        glm::vec3(0.5f, 0.2f, 0.6f), // Purple
        glm::vec3(0.2f, 0.6f, 0.7f), // Cyan
        glm::vec3(0.8f, 0.5f, 0.3f)  // Orange
    };

    for (size_t i = 0; i < Meshes.size(); ++i) {
        glm::mat4 localModelMatrix = ModelMatrices[i];
        glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(localModelMatrix));

        // Set the base color for the current mesh
        glUniform3fv(Shaders->Uniforms["baseColor"].index, 1, glm::value_ptr(baseColors[i]));

        Meshes[i]->draw();
    }

    Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (Camera) Camera->onMouseMove(win, xpos, ypos);
}

void MyApp::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    if (Camera) Camera->onScroll(win, xoffset, yoffset);
}

void MyApp::initCallback(GLFWwindow* win) {
    createMeshes();
    createShaderPrograms();  // after mesh;
    createCamera();
}

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
    glViewport(0, 0, winx, winy);
    // change projection matrices to maintain aspect ratio
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) { drawScene(); }

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
    mgl::Engine& engine = mgl::Engine::getInstance();
    engine.setApp(new MyApp());
    engine.setOpenGL(4, 6);
    engine.setWindow(800, 600, "Mesh Loader", 0, 1);
    engine.init();
    engine.run();
    exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////