#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window_p, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window_p) {
    if (glfwGetKey(window_p, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_p, true);
    }
}

GLFWwindow* initializeGLFWandGLAD() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Attempt to create GLFW window
    GLFWwindow *window_p = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window_p == NULL) {
        std::cerr << "Failed to create glfw window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window_p);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // Set OpenGL viewport size
    glViewport(0, 0, 800, 600);
    // Assign a callback to update the OpenGL viewport size whenever the
    // GLFW window resizes
    glfwSetFramebufferSizeCallback(window_p, framebuffer_size_callback);

    return window_p;
}

int main() {
    GLFWwindow *window_p = initializeGLFWandGLAD();
    if (window_p == NULL) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    float verticies[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Create VBO Buffer
    // This is the section of memory we use
    // to pass data to the GPU.
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // Let OpenGL know we wish to work with our VBO Buffer
    // glBindBuffer lets openGL know which buffer you wish to work with
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy verticies into the currently bound buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    // Application Loop
    while(!glfwWindowShouldClose(window_p)) {
        // Process user input
        processInput(window_p);
        
        // Clear screen
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap color buffers
        glfwSwapBuffers(window_p);

        // Poll events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}