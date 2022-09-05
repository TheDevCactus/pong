#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>

/**
 * @brief Update OpenGL Viewport size through GLFW callback
 * 
 * @param window_p 
 * @param width 
 * @param height 
 */
void framebuffer_size_callback(GLFWwindow *window_p, int width, int height) {
    glViewport(0, 0, width, height);
}

/**
 * @brief Process user input 
 * 
 * @param window_p 
 */
void processInput(GLFWwindow *window_p) {
    if (glfwGetKey(window_p, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_p, true);
    }
}

/**
 * @brief Initialize GLFW and GLAD
 * This creates our GLFW window, initializes OpenGL, and binds our
 * frame buffer resize callback.
 * 
 * @return GLFWwindow* 
 */
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

/**
 * @brief Reads a file into a std::string. Returns true if
 * file was succesfully read, otherwise returns false
 * 
 * @param filePath 
 * @param fileContents 
 * @return true 
 * @return false 
 */
bool readFileToString(std::string filePath, std::string &fileContents) {
    // Open the file
    std::ifstream inFileStream;
    std::string line;
    inFileStream.open(filePath);

    // If the file is not open, throw error, return
    if (!inFileStream.is_open()) {
        std::cerr << "Error while attempting to open file: " << filePath << std::endl;
        return false;
    }

    // While there is more of the file to read,
    // read a line and append it to our output
    while(getline(inFileStream, line)) {
        fileContents.append(line + '\n');
    }

    // Close the file, and signify we successfully read the file
    inFileStream.close();
    return true;
}

/**
 * @brief Reads a shader source file, and creates a shader
 * from that source code. Returns true on success, otherwise
 * returns false.
 * 
 * @param filePath 
 * @param shaderType 
 * @param shader 
 * @return true 
 * @return false 
 */
bool initializeShaderFromFile(std::string filePath, GLenum shaderType, unsigned int &shader) {
    // Read shader source file
    std::string fileSource;
    if (!readFileToString(filePath, fileSource)) {
        return false;
    }

    // Create and compile the shader
    shader = glCreateShader(shaderType);
    const char *shaderSource = fileSource.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // Check if compilation was succesfull
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Failed to compile shader\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Create a Shader Program object, and delete shaders provided.
 * 
 * @param shaders 
 * @param shaderProgram 
 * @return true 
 * @return false 
 */
bool createShaderProgram(unsigned int shaders[], unsigned int &shaderProgram) {
    shaderProgram = glCreateProgram();

    int shaderCount = sizeof(shaders) / sizeof(unsigned int);
    for (int i = 0; i < shaderCount; i++) {
        glAttachShader(shaderProgram, shaders[i]);
        glDeleteShader(shaders[i]);     
    }
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Failed to create shader program\n" << infoLog << std::endl;
        return false;
    }

    return true;
}

int main() {
    GLFWwindow *window_p = initializeGLFWandGLAD();
    if (window_p == NULL) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    unsigned int vertexShader, fragmentShader;
    if (
        !initializeShaderFromFile("src/shaders/vertexShader.GLSL", GL_VERTEX_SHADER, vertexShader) ||
        !initializeShaderFromFile("src/shaders/fragmentShader.GLSL", GL_FRAGMENT_SHADER, fragmentShader)
    ) {
        return -1;
    }

    // Create and use our shader program
    unsigned int shaderProgram;
    unsigned int shaders[2] = {vertexShader, fragmentShader};
    if (!createShaderProgram(shaders, shaderProgram)) {
        return -1;
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

    // Create our VAO
    // This is how we store vertex attributes
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Let OpenGL know we wish to work with the specified VAO
    glBindVertexArray(VAO);
    // Let OpenGL know we wish to work with our VBO Buffer
    // glBindBuffer lets openGL know which buffer you wish to work with
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Specify how OpenGL should interpret our vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Copy verticies into the currently bound buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    // Tell OpenGL to use that specification
    glEnableVertexAttribArray(0);

    // Application Loop
    while(!glfwWindowShouldClose(window_p)) {
        // Process user input
        processInput(window_p);
        
        // Clear screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
        // Use our shader program we created earlier
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap color buffers
        glfwSwapBuffers(window_p);

        // Poll events
        glfwPollEvents();
        verticies[1] += 0.005f;
        verticies[4] += 0.005f;
        verticies[7] += 0.005f;

        verticies[0] += 0.005f;
        verticies[3] += 0.005f;
        verticies[6] += 0.005f;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}