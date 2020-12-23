#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Window dim
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

// ID
GLuint VAO, VBO, shader, uniformModel;

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
static const char* vShader = "                                        \n\
#version 330                                                          \n\
                                                                      \n\
layout (location = 0) in vec3 pos;                                    \n\
                                                                      \n\
uniform mat4 model;                                                   \n\
                                                                      \n\
void main()                                                           \n\
{                                                                     \n\
  gl_Position = model * vec4(pos, 1.0);                               \n\
}";

// Fragment Shader
// Most important output is the colour of the pixel
// that the fragment covers.
static const char* fShader = "                                        \n\
#version 330                                                          \n\
                                                                      \n\
out vec4 colour;                                                      \n\
                                                                      \n\
void main()                                                           \n\
{                                                                     \n\
  colour = vec4(1.0, 0.0, 0.0, 1.0);                                  \n\
}";

void CreateTriangle()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, // x, y ,z
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // x, y, z -> 3 value a vertex
    // location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // unbind VAO, VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024]{ 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
    }

    glAttachShader(theProgram, theShader);
}

void CompileShader() {
    // Programe sits on graphic card

    // id
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader programe!\n");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024]{ 0 };

    // Link
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
    }

    // Validate
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
    }

    // Get uniform var location
    uniformModel = glGetUniformLocation(shader, "model");
}

int main()
{
    // Init GLFW
    if (!glfwInit()) {
        printf("GLFW Init failed!");
        glfwTerminate();
        return 1;
    }

    // Set GLFW properties
    // OpenGL version
    // 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // No backward compatible
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test", NULL, NULL);

    if (!mainWindow) {
        printf("Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get buffer size info (buffer that the actual draw happens)
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    // (OpenGL context ties/ draws)
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        printf("GLEW init failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Setup Viewport size (gl)
    // sets up the size of the part we draw to on our window
    // entire window
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CompileShader();

    // Loop until window closes
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Get + handle user input events
        glfwPollEvents();

        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxOffset)
            direction = !direction;

        currentAngle += 0.001f;
        if (currentAngle >= 360.0f)
            currentAngle -= 360;

        if (sizeDirection) {
            currentSize += 0.0001f;
        }
        else {
            currentSize -= 0.0001f;
        }

        if (currentSize >= maxSize || currentSize <= minSize)
            sizeDirection = !sizeDirection;

        // Clear window (buffer cannot be seen)
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glUseProgram(shader);

        // Identity matrix
        glm::mat4 model(1.0f);

        // Translation x value
        //model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));

        // The distortion is because of lacking Projection matrix
        //model = glm::rotate(model, currentAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

        // x, y scale by 2
        model = glm::scale(model, glm::vec3(currentSize, currentSize, 1.0f));

        // Set uniform var
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        // Unassign the shader
        glUseProgram(0);

        // triple/two buffer (buffer that can be seen)
        glfwSwapBuffers(mainWindow);
    }

    return 0;
}