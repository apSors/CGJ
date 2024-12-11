////////////////////////////////////////////////////////////////////////////////
//
// Loading meshes from external files
//
// Copyright (c) 2023-24 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../mgl/mgl.hpp"

#include <vector>

class SceneNode {
public:
    SceneNode(mgl::Mesh* mesh = nullptr, glm::vec3 color = glm::vec3(1.0f))
        : parent(nullptr), localTransform(glm::mat4(1.0f)), worldTransform(glm::mat4(1.0f)),
          mesh(mesh), color(color) {}

    void setLocalTransform(const glm::mat4& transform) { localTransform = transform; }
    const glm::mat4& getWorldTransform() const { return worldTransform; }

    void setColor(const glm::vec3& col) { color = col; }

    void addChild(SceneNode* child) {
        children.push_back(child);
        child->parent = this;
    }

    void updateTransform() {
        if (parent) {
            worldTransform = parent->worldTransform * localTransform;
        } else {
            worldTransform = localTransform;
        }
        for (SceneNode* child : children) {
            child->updateTransform();
        }
    }

    void draw(mgl::ShaderProgram* shader, GLint modelMatrixId, GLint colorId) {
        if (mesh) {
            glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, glm::value_ptr(worldTransform));
            glUniform3fv(colorId, 1, glm::value_ptr(color));
            mesh->draw();
        }
        for (SceneNode* child : children) {
            child->draw(shader, modelMatrixId, colorId);
        }
    }

private:
    SceneNode* parent;
    glm::mat4 localTransform;
    glm::mat4 worldTransform;
    mgl::Mesh* mesh;
    glm::vec3 color;
    std::vector<SceneNode*> children;
};


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
    GLint ModelMatrixId;
    std::vector<glm::mat4> BoxModelMatrices;
    std::vector<glm::mat4> ModelMatrices;
    std::vector<mgl::Mesh*> Meshes;

    glm::mat4 originalOrthographicMatrix;
    glm::mat4 originalPerspectiveMatrix;
    bool isUsingPerspective = true;

    glm::mat4 viewMatrix;
    glm::mat4 alternateViewMatrix;
    bool isusingSecondCamera = false;

    SceneNode* tangramRoot;
    std::vector<SceneNode*> tangramPieces;

    void createMeshes();
    void createShaderPrograms();
    void createCamera();
    void switchCamera();
    void drawScene();
    void createSceneGraph();

    float animationProgress = 0.0f;
    bool isAnimatingForward = false;
    bool isAnimating = false;
};


void MyApp::createSceneGraph() {
    tangramRoot = new SceneNode();
    std::vector<glm::vec3> colors = {
        glm::vec3(0.7f, 0.3f, 0.2f), // Red
        glm::vec3(0.4f, 0.7f, 0.3f), // Green
        glm::vec3(0.2f, 0.3f, 0.8f), // Blue
        glm::vec3(0.7f, 0.8f, 0.0f), // Yellow
        glm::vec3(0.5f, 0.2f, 0.6f), // Purple
        glm::vec3(0.2f, 0.6f, 0.7f), // Cyan
        glm::vec3(0.8f, 0.5f, 0.3f)  // Orange
    };

    for (size_t i = 0; i < Meshes.size(); ++i) {
        SceneNode* pieceNode = new SceneNode(Meshes[i], colors[i]);
        //pieceNode->setLocalTransform(BoxModelMatrices[i]);
        tangramRoot->addChild(pieceNode);
        tangramPieces.push_back(pieceNode);
    }
    tangramRoot->updateTransform();
}


///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_dir = "../assets/";

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
    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

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

void MyApp::createCamera() {
    int winx, winy;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &winx, &winy);
    float aspectRatio = static_cast<float>(winx) / static_cast<float>(winy);

    float orthoHeight = 5.0f;
    float orthoWidth = orthoHeight * aspectRatio;

    originalOrthographicMatrix = glm::ortho(
        -orthoWidth, orthoWidth,
        -orthoHeight, orthoHeight,
        -10.0f, 100.0f
    );

    originalPerspectiveMatrix = glm::perspective(
        glm::radians(100.0f), aspectRatio, 1.0f, 50.0f
    );

    Camera = new mgl::Camera(UBO_BP);
    Camera->setProjectionMatrix(originalPerspectiveMatrix, true);

    viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    alternateViewMatrix = glm::lookAt(
        glm::vec3(0.0f, 5.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    Camera->setViewMatrix(viewMatrix);
}

void MyApp::switchCamera() {
    static glm::vec3 primaryCameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    static glm::quat primaryCameraOrientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    static glm::mat4 primaryProjectionMatrix = originalPerspectiveMatrix;

    static glm::vec3 alternateCameraPosition = glm::vec3(0.0f, 10.0f, 0.0f);
    static glm::quat alternateCameraOrientation = glm::quat(glm::vec3(-0.3f, 0.3f, 0.0f));
    static glm::mat4 alternateProjectionMatrix = originalOrthographicMatrix;

    if (isusingSecondCamera) {
        alternateCameraPosition = Camera->getPosition();
        alternateCameraOrientation = Camera->getOrientation();
        alternateProjectionMatrix = Camera->getProjectionMatrix();

        Camera->setPosition(primaryCameraPosition);
        Camera->setOrientation(primaryCameraOrientation);
        Camera->setProjectionMatrix(primaryProjectionMatrix, isUsingPerspective);
    }
    else {
        primaryCameraPosition = Camera->getPosition();
        primaryCameraOrientation = Camera->getOrientation();
        primaryProjectionMatrix = Camera->getProjectionMatrix();

        Camera->setPosition(alternateCameraPosition);
        Camera->setOrientation(alternateCameraOrientation);
        Camera->setProjectionMatrix(alternateProjectionMatrix, !isUsingPerspective);
    }
    isusingSecondCamera = !isusingSecondCamera;
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
        animationProgress += (isAnimatingForward ? 0.01f : -0.01f);
        animationProgress = glm::clamp(animationProgress, 0.0f, 1.0f);

        for (size_t i = 0; i < tangramPieces.size(); ++i) {
            glm::mat4 interpolatedTransform = interpolate(
                ModelMatrices[i], BoxModelMatrices[i], animationProgress
            );
            tangramPieces[i]->setLocalTransform(interpolatedTransform);
        }

        tangramRoot->updateTransform();
    }

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

    Shaders->bind();
    GLint colorId = Shaders->Uniforms["baseColor"].index;
    tangramRoot->draw(Shaders, ModelMatrixId, colorId); 
    Shaders->unbind();
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_C) {
            switchCamera();
        }
        if (key == GLFW_KEY_P) {
            if (Camera->getProjectionMatrix() == originalPerspectiveMatrix) {
                Camera->setProjectionMatrix(originalOrthographicMatrix, false);
                isUsingPerspective = false;
            }
            else {
                Camera->setProjectionMatrix(originalPerspectiveMatrix, true);
                isUsingPerspective = true;
            }
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
    
}

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
    createSceneGraph();
}

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
    glViewport(0, 0, winx, winy);
    float aspectRatio = static_cast<float>(winx) / static_cast<float>(winy);

    float orthoHeight = 10.0f;
    float orthoWidth = orthoHeight * aspectRatio;

    originalOrthographicMatrix = glm::ortho(
        -orthoWidth, orthoWidth,
        -orthoHeight, orthoHeight,
        -10.0f, 100.0f
    );

    originalPerspectiveMatrix = glm::perspective(
        glm::radians(100.0f), aspectRatio, 1.0f, 50.0f
    );

    if (isUsingPerspective) {
        Camera->setProjectionMatrix(originalPerspectiveMatrix, true);
    }
    else {
        Camera->setProjectionMatrix(originalOrthographicMatrix, false);
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