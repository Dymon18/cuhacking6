#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <filesystem>  // C++17 feature
#include "stb_image.h"

namespace fs = std::filesystem;

// Window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Vertex Shader (GLES)
const char* vertexShaderSource = R"(
    #version 300 es
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

// Fragment Shader (GLES)
const char* fragmentShaderSource = R"(
    #version 300 es
    precision mediump float;
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
        FragColor = texture(texture1, TexCoord);
    }
)";

// Load Texture
GLuint loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    if (data) {
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

// Generate Image Filename
std::string generateImageFilename(int index) {
    std::ostringstream filename;
    filename << "pngfolder/" << std::setw(4) << std::setfill('0') << index << ".png";
    return filename.str();
}

// Count PNG Files in Folder
int countPngFiles(const std::string& folderPath) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".png") {
            count++;
        }
    }
    return count;
}

// Initialize EGL and Create Window
EGLDisplay initializeEGL(EGLContext& eglContext, EGLSurface& eglSurface) {
    EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(eglDisplay, NULL, NULL);

    EGLint configAttributes[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    EGLConfig eglConfig;
    EGLint numConfigs;
    eglChooseConfig(eglDisplay, configAttributes, &eglConfig, 1, &numConfigs);

    EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttributes);
    
    // Create window surface (X11, Wayland, Windows, etc.)
    EGLNativeWindowType nativeWindow = 0;  // Replace with platform-specific code
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nativeWindow, NULL);
    
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    return eglDisplay;
}

int main() {
    int TOTAL_IMAGES = countPngFiles("pngfolder");

    // Initialize EGL
    EGLContext eglContext;
    EGLSurface eglSurface;
    EGLDisplay eglDisplay = initializeEGL(eglContext, eglSurface);

    // Load first texture
    int currentImageIndex = 0;
    GLuint texture = loadTexture(generateImageFilename(currentImageIndex));

    // Vertex Data
    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
        -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,  1.0f, 1.0f  // Top-right
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    // Create Buffers
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

    // Compile Shaders
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

    double lastSwitchTime = 0.0;
    const double frameDelay = 1.0 / 45.0;

    // Render Loop
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        double currentTime = eglGetProcAddress("eglGetTime");
        if (currentTime - lastSwitchTime >= frameDelay) {
            lastSwitchTime = currentTime;
            currentImageIndex = (currentImageIndex + 1) % TOTAL_IMAGES;
            glDeleteTextures(1, &texture);
            texture = loadTexture(generateImageFilename(currentImageIndex));
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        eglSwapBuffers(eglDisplay, eglSurface);
    }

    eglTerminate(eglDisplay);
    return 0;
}
