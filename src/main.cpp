#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Position {
    float x;
    float y;
    float z;
};

struct Renderable {
    std::vector<float> vertices;
    unsigned int VBO;
    unsigned int VAO;
};

struct GameObject {
    std::string id;
    Renderable renderable;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec2 size;
};

struct Camera {
    glm::mat4 view;
    glm::vec3 position;
    glm::mat4 projection;
};

void framebuffer_size_callback(GLFWwindow *window_p, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow* initializeGLFWandGLAD() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Attempt to create GLFW window
    GLFWwindow *window_p = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "PONG | Morgan Brown", NULL, NULL);
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
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Assign a callback to update the OpenGL viewport size whenever the
    // GLFW window resizes
    glfwSetFramebufferSizeCallback(window_p, framebuffer_size_callback);

    return window_p;
}

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

bool readAssetFile(std::string assetFilePath, std::vector<float> &verticies, glm::vec2 &size) {
    std::string fileSource;
    if (!readFileToString(assetFilePath, fileSource)) {
        return false;
    };
    std::string currentFloatStr;

    size[0] = 0;
    size[1] = 0;
    for (int i = 0; i < fileSource.size(); i ++) {
        switch (fileSource[i]) {
            case ',': 
            case '\n': {
                if (currentFloatStr == "") {
                    continue;
                }
                float currentFloat = atof(currentFloatStr.c_str());
                if (verticies.size() % 2 == 0) {
                    if (currentFloat > size[0]) {
                        size[0] = currentFloat;
                    }
                } else {
                    if (currentFloat > size[1]) {
                        size[1] = currentFloat;
                    }
                }
                verticies.push_back(currentFloat);
                currentFloatStr = "";
                break;
            }
            default: {
                currentFloatStr.push_back(fileSource[i]);
            }
        }
    }

    return true;
}

bool createShaderProgram(unsigned int shaders[], unsigned int shaderCount, unsigned int &shaderProgram) {
    shaderProgram = glCreateProgram();

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

Renderable createRenderable(float vertices[], unsigned int numberOfVerticies) {
    float floatsPerVertexPoint = 2;
    Renderable newRenderable;

    for (int i = 0; i < numberOfVerticies; i ++) {
        newRenderable.vertices.push_back(vertices[i]);
    }

    glGenBuffers(1, &newRenderable.VBO);
    glGenVertexArrays(1, &newRenderable.VAO);

    glBindVertexArray(newRenderable.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newRenderable.VBO);

    glVertexAttribPointer(
        0, 
        floatsPerVertexPoint, 
        GL_FLOAT, 
        GL_FALSE, 
        floatsPerVertexPoint * sizeof(float), 
        (void*)0
    );
    glEnableVertexAttribArray(0);
    return newRenderable;
}

bool createGameObject(std::string id, std::string assetFilePath, GameObject &gameObject) {
    std::vector<float> assetVerticies;
    if (!readAssetFile(assetFilePath, assetVerticies, gameObject.size)) {
        return false;
    }
    
    gameObject.renderable = createRenderable(&assetVerticies[0], assetVerticies.size());
    gameObject.id = id;
    gameObject.position = glm::vec3(0.0f);
    gameObject.rotation = glm::vec3(0.0f);

    return true;
}

Camera createCamera() {
    Camera out;

    out.view = glm::mat4(1.0f);
    out.position = glm::vec3(0.0f, 0.0f, -1.0f);
    out.view = glm::translate(out.view, out.position);

    out.projection = glm::ortho(
        0.0f,                       // Left
        (float)WINDOW_WIDTH,        // Right
        0.0f,                       // Bottom
        (float)WINDOW_HEIGHT,       // Top
        0.0f,                       // Near Clip
        100.0f                      // Far Clip
    );

    return out;
};

void renderRenderable(Renderable renderable) {
    float verticesToRender[renderable.vertices.size()];
    std::copy(renderable.vertices.begin(), renderable.vertices.end(), verticesToRender);

    glBindVertexArray(renderable.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesToRender), verticesToRender, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, (sizeof(verticesToRender) / sizeof(float)) / 2); 
}

void renderGameObject(GameObject &gameObject, unsigned int modelUniformLocation) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, gameObject.position);
    glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
    renderRenderable(gameObject.renderable);
}

void deleteRenderable(Renderable renderable) {
    glDeleteBuffers(1, &renderable.VBO);
    glDeleteVertexArrays(1, &renderable.VAO);
}

bool doGameObjectsCollide(GameObject &objectA, GameObject &objectB) {
    if (
        objectA.position[0] + objectA.size[0] >= objectB.position[0] &&
        objectA.position[0] <= objectB.position[0] + objectB.size[0] &&
        objectA.position[1] + objectA.size[1] >= objectB.position[1] &&
        objectA.position[1] <= objectB.position[1] + objectB.size[1]
    ) {
        return true;
    }
    return false;
}

int main() {

    GLFWwindow *window_p = initializeGLFWandGLAD();
    if (window_p == NULL) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
    }

    unsigned int vertexShader, fragmentShader;
    if (
        !initializeShaderFromFile("shaders/vertexShader.GLSL", GL_VERTEX_SHADER, vertexShader) ||
        !initializeShaderFromFile("shaders/fragmentShader.GLSL", GL_FRAGMENT_SHADER, fragmentShader)
    ) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }
    
    // Create and use our shader program
    unsigned int shaderProgram;
    unsigned int shaders[2] = {vertexShader, fragmentShader};
    if (!createShaderProgram(shaders, 2, shaderProgram)) {
        return -1;
    }
    glUseProgram(shaderProgram);

    Camera camera = createCamera();
    GameObject paddleA, paddleB, ball;

    createGameObject("paddle-a", "assets/paddle.v", paddleA);
    createGameObject("paddle-b", "assets/paddle.v", paddleB);
    createGameObject("ball", "assets/ball.v", ball);

    unsigned int modelUniformLocation = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewUniformLocation = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionUniformLocation = glGetUniformLocation(shaderProgram, "projection");

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));

    glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, glm::value_ptr(camera.view));
    glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(camera.projection));

    paddleA.position[1] += 10.0f;

    paddleB.position[1] += 585.0f;

    ball.position[1] = 300.0f;
    ball.position[0] = 400.0f;

    float paddleSpeed = 400.0f;
    float ballSpeed = 200.0f;
    bool gameStarted = false;
    glm::vec2 ballForce = glm::vec2(0.0f, 0.0f);

    float time = glfwGetTime();
    float lastFrame = glfwGetTime();
    int frames = 0;

    glfwSwapInterval(0);

    // Application Loop
    while(!glfwWindowShouldClose(window_p)) {
        float currentTime = glfwGetTime();
        float deltaTime = glfwGetTime() - lastFrame;

        // Track FPS
        {
            ++frames;
            if (currentTime - 1 > time) {
                std::cout << "FPS: " << frames << std::endl;
                frames = 0;
                time = currentTime;
            }
        }

        // Process user input
        {
            // Start Game - SPACE BAR
            if (glfwGetKey(window_p, GLFW_KEY_SPACE) == GLFW_PRESS) {
                gameStarted = true;
                ballForce[1] = ballSpeed;
            }

            float adjustedPaddleSpeed = paddleSpeed * deltaTime;
            // Move Paddle B - A & D
            if (glfwGetKey(window_p, GLFW_KEY_A) == GLFW_PRESS) {
                if (paddleB.position[0] < adjustedPaddleSpeed) {
                    paddleB.position[0] = 0.0f;
                } else {
                    paddleB.position[0] -= adjustedPaddleSpeed; 
                }
            }
            if (glfwGetKey(window_p, GLFW_KEY_D) == GLFW_PRESS) {
                if (paddleB.position[0] > WINDOW_WIDTH - paddleB.size[0] - adjustedPaddleSpeed) {
                    paddleB.position[0] = WINDOW_WIDTH - paddleB.size[0];
                } else {
                    paddleB.position[0] += adjustedPaddleSpeed;
                }
            }

            // Move Paddle A - LEFT & RIGHT
            if (glfwGetKey(window_p, GLFW_KEY_LEFT) == GLFW_PRESS) {
                if (paddleA.position[0] < adjustedPaddleSpeed) {
                    paddleA.position[0] = 0.0f;
                } else {
                    paddleA.position[0] -= adjustedPaddleSpeed;
                }
            }
            if (glfwGetKey(window_p, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                if (paddleA.position[0] > WINDOW_WIDTH - paddleA.size[0] - adjustedPaddleSpeed) {
                    paddleA.position[0] = WINDOW_WIDTH - paddleA.size[0];
                } else {
                    paddleA.position[0] += adjustedPaddleSpeed;
                }
            }
        }

        // Move Ball       
        {
            // Win conditions
            {
                if (ball.position[1] <= 0) {
                    std::cout << "PLAYER B WINS" << std::endl;
        
                    glfwTerminate();
                    return 0;
                }

                if (ball.position[1] >= WINDOW_HEIGHT) {
                    std::cout << "PLAYER A WINS" << std::endl;

                    glfwTerminate();
                    return 0;
                }
            }

            if (
                ball.position[0] <= 0 ||
                ball.position[0] >= WINDOW_WIDTH - ball.size[0]
            ) {
                ballForce[0] *= -1;
            }

            if (doGameObjectsCollide(ball, paddleB)) {
                float paddleCenter = paddleB.position[0] + (paddleB.size[0] / 2);
                float leftPaddleBound = paddleB.position[0];
                float rightPaddleBound = paddleB.position[0] + paddleB.size[0];
                float ballCenter = ball.position[0] + (ball.size[0] / 2);

                // TODO: something is wrong here. ball bounces in wrong direction given the side of the paddle it hits
                if (ballCenter < paddleCenter) {
                    ballForce[0] = ((paddleCenter - leftPaddleBound)/(ballCenter - leftPaddleBound)) * -100;
                } else {
                    ballForce[0] = ((rightPaddleBound - paddleCenter)/(ballCenter - rightPaddleBound)) * -100;
                }

                ballForce[1] *= -1;
            }
            if (doGameObjectsCollide(ball, paddleA)) {
                float paddleCenter = paddleA.position[0] + (paddleA.size[0] / 2);
                float leftPaddleBound = paddleA.position[0];
                float rightPaddleBound = paddleA.position[0] + paddleA.size[0];
                float ballCenter = ball.position[0] + (ball.size[0] / 2);

                // TODO: something is wrong here. ball bounces in wrong direction given the side of the paddle it hits
                if (ballCenter < paddleCenter) {
                    ballForce[0] = ((ballCenter - leftPaddleBound) / (paddleCenter - leftPaddleBound)) * -100;
                } else {
                    ballForce[0] = ((ballCenter - rightPaddleBound) / (rightPaddleBound - paddleCenter)) * -100;
                }

                ballForce[1] *= -1;
            }

            ball.position[0] += ballForce[0] * deltaTime;
            ball.position[1] += ballForce[1] * deltaTime;
        }

        // Render
        {
            // Clear screen
            glClearColor(0.1f, 0.9f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            renderGameObject(paddleA, modelUniformLocation);
            renderGameObject(paddleB, modelUniformLocation);
            renderGameObject(ball, modelUniformLocation);

            // // Swap color buffers
            glfwSwapBuffers(window_p);

            // Poll events
            glfwPollEvents();
        }
        lastFrame = currentTime;
    }

    // deleteRenderable(paddle);
    // deleteRenderable(paddleB);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}