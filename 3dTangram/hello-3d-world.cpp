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
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../mgl/mgl.hpp"

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
    void initCallback(GLFWwindow* win) override;
    void displayCallback(GLFWwindow* win, double elapsed) override;
    void windowSizeCallback(GLFWwindow* win, int winx, int winy) override;
    void cursorCallback(GLFWwindow* win, double xpos, double ypos);
    void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);
    void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods);

private:
    const GLuint UBO_BP = 0;
    mgl::ShaderProgram* Shaders = nullptr;
    mgl::Camera* Camera = nullptr;
    mgl::Camera* Camera2 = nullptr;
    mgl::Camera* currentCamera = nullptr;
    GLint ModelMatrixId;
    std::vector<glm::mat4> BoxModelMatrices;
    std::vector<glm::mat4> ModelMatrices;
    std::vector<mgl::Mesh*> Meshes;

    void createMeshes();
    void createShaderPrograms();
    void createCameras();
    void drawScene();

    float animationProgress = 0.0f;
    bool isAnimatingForward = false;
    bool isAnimating = false;

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

    ModelMatrices.clear();
    BoxModelMatrices.clear();
    Meshes.clear();


    for (size_t i = 0; i < mesh_files.size(); ++i) {
        std::string mesh_fullname = mesh_dir + mesh_files[i];

        mgl::Mesh* mesh = new mgl::Mesh();
        mesh->joinIdenticalVertices();
        mesh->create(mesh_fullname);

        Meshes.push_back(mesh);

        glm::mat4 transformation = glm::mat4(1.0f);
        glm::mat4 localRotation = glm::mat4(1.0f);
        ModelMatrices.push_back(transformation);

        glm::mat4 boxTransformation = glm::mat4(1.0f);

        switch (i) {
        case 0:  // Medium Triangle
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(-2.0f, 0.0f, 1.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;

        case 1:  // Large Triangle (green)
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(sqrt(2) / 2, 0.0f, 2 - 3 * sqrt(2) / 2));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;

        case 2:  // Large Triangle (blue)
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(-1 + sqrt(2), 0.0f, 4 - sqrt(2)));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;

        case 3:  // Square
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(-1.0f, 0.0f, 4 + sqrt(2)));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            break;

        case 4:  // Small Triangle (purple)
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(2.0f, 0.0f, sqrt(2)));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;

        case 5:  // Small Triangle (cyan)
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(-sqrt(2) / 2, 0.0f, 2.0f + sqrt(2) / 2));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;

        case 6:  // Parallelogram
            boxTransformation = glm::mat4(1.0f);
            boxTransformation = glm::translate(boxTransformation, glm::vec3(3 * sqrt(2) / 2, 0.0f, 2 - 5 * sqrt(2) / 2));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            boxTransformation = glm::rotate(boxTransformation, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        }

        BoxModelMatrices.push_back(boxTransformation);
    }
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    // Create and compile the shader program
    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, "3dTangram/cube-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "3dTangram/cube-fs.glsl");

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

    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniform("baseColor");
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);

    Shaders->create();

    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
}

///////////////////////////////////////////////////////////////////////// CAMERA

// Eye(5,5,5) Center(0,0,0) Up(0,1,0)
const glm::mat4 ViewMatrix1 =
glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

// Camera at (0.0f, 50.0f, 50.0f), even farther from the object
const glm::mat4 ViewMatrix2 =
glm::lookAt(glm::vec3(0.0f, 50.0f, 50.0f),  // Eye position farther from the object
    glm::vec3(0.0f, 0.0f, 0.0f),   // Center (looking towards the origin)
    glm::vec3(0.0f, 1.0f, 0.0f));  // Up vector (keeping the view upright)

// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
const glm::mat4 ProjectionMatrix1 =
glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);

// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
const glm::mat4 ProjectionMatrix2 =
glm::perspective(glm::radians(100.0f), 640.0f / 480.0f, 1.0f, 50.0f);

void MyApp::createCameras() {
    int winx, winy;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &winx, &winy); // Get actual window dimensions
    float aspectRatio = static_cast<float>(winx) / static_cast<float>(winy);
    
    Camera = new mgl::Camera(UBO_BP);

    // Use the actual aspect ratio for the initial projection
    float orthoHeight = 2.0f; // Fixed orthographic height
    float orthoWidth = orthoHeight * aspectRatio;

    Camera->setProjectionMatrix(glm::ortho(
        -orthoWidth, orthoWidth,  // Left, Right
        -orthoHeight, orthoHeight, // Bottom, Top
        1.0f, 10.0f               // Near, Far
    ), false);

    glm::mat4 initialViewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f), // Eye position
        glm::vec3(0.0f, 0.0f, 0.0f), // Target position
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
    );
    Camera->setViewMatrix(initialViewMatrix);
 
 
     /*
    
    // Create the second camera
    Camera2 = new mgl::Camera(UBO_BP);
    Camera2->setProjectionMatrix(ProjectionMatrix2, true);
    const glm::mat4 initialViewMatrix2 = glm::lookAt(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Camera2->setViewMatrix(initialViewMatrix2);*/
    // Set the initial active camera
    currentCamera = Camera; 
}
/////////////////////////////////////////////////////////////////////////// DRAW

glm::mat4 ModelMatrix(1.0f);

glm::mat4 interpolate(const glm::mat4& start, const glm::mat4& end, float alpha) {
    glm::vec3 startScale, startTrans, startSkew;
    glm::quat startRot;
    glm::vec4 startPerspective;
    glm::decompose(start, startScale, startRot, startTrans, startSkew, startPerspective);

    glm::vec3 endScale, endTrans, endSkew;
    glm::quat endRot;
    glm::vec4 endPerspective;
    glm::decompose(end, endScale, endRot, endTrans, endSkew, endPerspective);

    glm::vec3 interpolatedTrans = glm::mix(startTrans, endTrans, alpha);
    glm::quat interpolatedRot = glm::slerp(startRot, endRot, alpha);
    glm::vec3 interpolatedScale = glm::mix(startScale, endScale, alpha);

    glm::mat4 interpolatedMat = glm::translate(glm::mat4(1.0f), interpolatedTrans) *
        glm::mat4_cast(interpolatedRot) *
        glm::scale(glm::mat4(1.0f), interpolatedScale);

    return interpolatedMat;
}

void MyApp::drawScene() {
    if (isAnimating) {
        if (isAnimatingForward) {
            animationProgress += 0.01f;
            if (animationProgress > 1.0f) {
                animationProgress = 1.0f;
                isAnimating = false;
            }
        }
        else {
            animationProgress -= 0.01f;
            if (animationProgress < 0.0f) {
                animationProgress = 0.0f;
                isAnimating = false;
            }
        }
    }

    Shaders->bind();

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
        glm::mat4 interpolatedMatrix = interpolate(ModelMatrices[i], BoxModelMatrices[i], animationProgress);

        glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(interpolatedMatrix));

        glUniform3fv(Shaders->Uniforms["baseColor"].index, 1, glm::value_ptr(baseColors[i]));

        Meshes[i]->draw();
    }

    Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        if (currentCamera == Camera) {
            currentCamera = Camera2;
        } else {
            currentCamera = Camera;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            isAnimating = true;
            isAnimatingForward = false;
        }
        else if (action == GLFW_RELEASE) {
            isAnimating = false;
        }
    }
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            isAnimating = true;
            isAnimatingForward = true;
        }
        else if (action == GLFW_RELEASE) {
            isAnimating = false;
        }
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        int winx, winy;
        glfwGetFramebufferSize(win, &winx, &winy);
        float aspectRatio = static_cast<float>(winx) / static_cast<float>(winy);

        if (currentCamera->getIsPerspective()) {
            // Switch to orthographic projection
            float orthoHeight = 2.0f;
            float orthoWidth = orthoHeight * aspectRatio;

            currentCamera->setProjectionMatrix(glm::ortho(
                -orthoWidth, orthoWidth,
                -orthoHeight, orthoHeight,
                1.0f, 10.0f
            ), false);
        }
        else {
            // Switch to perspective projection
            currentCamera->setProjectionMatrix(glm::perspective(
                glm::radians(100.0f), aspectRatio, 1.0f, 50.0f
            ), true);
        }
    }
}

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (currentCamera) currentCamera->onMouseMove(win, xpos, ypos);
}

void MyApp::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    if (currentCamera) currentCamera->onScroll(win, xoffset, yoffset);
}

void MyApp::initCallback(GLFWwindow* win) {
    createMeshes();
    createShaderPrograms();  // after mesh;
    createCameras();
}

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
    glViewport(0, 0, winx, winy);

    // Compute the new aspect ratio
    float aspectRatio = static_cast<float>(winx) / static_cast<float>(winy);

    // Update the projection matrix for the current camera
    if (currentCamera) {
        if (currentCamera->getIsPerspective()) {
            currentCamera->setProjectionMatrix(glm::perspective(glm::radians(100.0f), aspectRatio, 1.0f, 50.0f), true);
        }
        else {
            // Orthographic projection matrix
            float orthoHeight = 2.0f; // Fixed height
            float orthoWidth = orthoHeight * aspectRatio; // Adjust width based on aspect ratio

            currentCamera->setProjectionMatrix(glm::ortho(
                -orthoWidth, orthoWidth,  // Left, Right
                -orthoHeight, orthoHeight, // Bottom, Top
                1.0f, 10.0f               // Near, Far
            ), false);
        }
    }
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