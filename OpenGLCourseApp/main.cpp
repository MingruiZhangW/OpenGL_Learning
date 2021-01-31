#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

// Window dim
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float currentAngle = 0.0f;

bool sizeDirection = true;
float currentSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Vertex Shader
// gl_Position built in (out), final pos for now
// pass to fragment shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
// Most important output is the colour of the pixel
// that the fragment covers.

// Here it interpolates the vertices to pixels
static const char* fShader = "Shaders/shader.frag";

void
CreateObject()
{
    // Vertex to use in order from vertices array
    // to draw a pyramid
    unsigned int indices[] = {0, 3, 1, 1, 3, 2, 2, 3, 0, 0, 1, 2};

    GLfloat vertices[] = {-1.0f,
                          -1.0f,
                          0.0f, // x, y ,z
                          0.0f,
                          -1.0f,
                          1.0f,
                          1.0f,
                          -1.0f,
                          0.0f,
                          0.0f,
                          1.0f,
                          0.0f};

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.emplace_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12);
    meshList.emplace_back(obj2);
}

void
CreateShader()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.emplace_back(shader1);
}

int
main()
{
    Window mainWindow(WIDTH, HEIGHT);
    mainWindow.Initialise();

    CreateObject();
    CreateShader();

    GLuint uniformProjection = 0, uniformModel = 0;

    // Perspective projection
    // 1: field of view: how wide our view is: 45 degrees
    // 2: Width of the window / height of the window
    // 3: Near field
    // 4: Far field
    glm::mat4 projection = glm::perspective(45.0f,
                                            mainWindow.getBufferWidth()
                                                / mainWindow.getBufferHeight(),
                                            0.1f,
                                            100.0f);

    // Loop until window closes
    while (!mainWindow.getShouldClose()) {
        // Get + handle user input events
        glfwPollEvents();

        if (direction) {
            triOffset += triIncrement;
        } else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxOffset)
            direction = !direction;

        currentAngle += 0.005f;
        if (currentAngle >= 360.0f)
            currentAngle -= 360;

        if (sizeDirection) {
            currentSize += 0.0001f;
        } else {
            currentSize -= 0.0001f;
        }

        if (currentSize >= maxSize || currentSize <= minSize)
            sizeDirection = !sizeDirection;

        // Clear window (buffer cannot be seen)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetModelLocation();
        uniformProjection = shaderList[0]->GetProjectionLocation();

        // Identity matrix
        glm::mat4 model(1.0f);

        // Translation z value to make sure that if does not get too close
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));

        // The distortion is because of lacking Projection matrix
        model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

        // x, y scale by 2
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

        // Set uniform var
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-triOffset, 0.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        meshList[1]->RenderMesh();

        // Unassign the shader
        Shader::UnUseShader();

        // triple/two buffer (buffer that can be seen)
        mainWindow.swapBuffers();
    }

    return 0;
}
