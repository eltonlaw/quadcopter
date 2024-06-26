// System Headers
#define GLEW_STATIC
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

const int mWidth = 800;
const int mHeight = 600;

namespace sim {


static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int render() {
    glfwSetErrorCallback(error_callback);

    // Load GLFW and Create a Window
    if (glfwInit() == false) {
        fprintf(stderr, "Failed to Initialize GLFW");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, "Simulator", nullptr, nullptr);

    // Check for Valid Context
    if (window == nullptr) {
        glfwTerminate();
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);
    fprintf(stdout, "OpenGL %s\n", glGetString(GL_VERSION));

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Check if any events are triggered and update windows state, calling
        // any corresponding callbacks and returns immediately
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

} // namespace sim
