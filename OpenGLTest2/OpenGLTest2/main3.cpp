#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <filesystem>  // C++17 for directory iteration
#include "stb_image.h"

namespace fs = std::filesystem;

// Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Vertex shader source
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Fragment shader source
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
        FragColor = texture(texture1, TexCoord);
    }
)";

// Load image into OpenGL texture
GLuint loadTexture(const std::string& path, float& aspectRatio) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    if (data) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// Get all PNG files from directory
std::vector<std::string> getPngFiles(const std::string& folderPath) {
    std::vector<std::string> pngFiles;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".png") {
            pngFiles.push_back(entry.path().string());
        }
    }
    return pngFiles;
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Render PNGs", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Get PNG files from the "pngfolder"
    std::vector<std::string> pngFiles = getPngFiles("pngfolder");
    if (pngFiles.empty()) {
        std::cerr << "No PNG images found in 'pngfolder'!" << std::endl;
        return -1;
    }

    // Load textures
    std::vector<GLuint> textures;
    std::vector<float> aspectRatios;
    for (const auto& file : pngFiles) {
        float aspectRatio;
        textures.push_back(loadTexture(file, aspectRatio));
        aspectRatios.push_back(aspectRatio);
    }

    // Compute aspect ratio scaling
    float windowAspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    std::vector<std::pair<float, float>> scales;
    for (const auto& aspectRatio : aspectRatios) {
        float scaleX = 1.0f, scaleY = 1.0f;
        if (aspectRatio > windowAspectRatio) {
            scaleY = windowAspectRatio / aspectRatio;
        } else {
            scaleX = aspectRatio / windowAspectRatio;
        }
        scales.push_back({scaleX, scaleY});
    }

    // Vertex data for centered quad
    float vertices[] = {
        // Positions       // Texture Coords
        -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
        -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,  1.0f, 1.0f  // Top-right
    };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    // Create buffers
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Render loop
    double lastSwitchTime = 0.0;
    int currentIndex = 0;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // Switch images every 1 second
        // FPS Tracking Variables
double lastTime = glfwGetTime();
int frameCount = 0;

while (!glfwWindowShouldClose(window)) {
    double currentTime = glfwGetTime();
    frameCount++;

    // If a second has passed, print FPS and reset count
    if (currentTime - lastTime >= 1.0) {
        std::cout << "FPS: " << frameCount << std::endl;
        frameCount = 0;
        lastTime = currentTime;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);

    // Switch images every 0.1 second (10 FPS for image cycling)
    if (currentTime - lastSwitchTime >= 0.1) {
        currentIndex = (currentIndex + 1) % textures.size();
        lastSwitchTime = currentTime;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[currentIndex]);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
}
    }


    // Cleanup
    for (GLuint texture : textures) {
        glDeleteTextures(1, &texture);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
