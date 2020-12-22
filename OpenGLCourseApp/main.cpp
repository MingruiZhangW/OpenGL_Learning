#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Window dim
const GLint WIDTH = 800, HEIGHT = 600;

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

    // Loop until window closes
    while (!glfwWindowShouldClose(mainWindow))
    {
        // Get + handle user input events
        glfwPollEvents();

        // Clear window (buffer cannot be seen)
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // triple/two buffer (buffer that can be seen)
        glfwSwapBuffers(mainWindow);
    }

    return 0;
}