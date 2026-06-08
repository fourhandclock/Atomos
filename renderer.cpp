#include <glad/glad.h> 
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "renderer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

GLFWwindow* InitWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "  [Init] -> Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    
    return window;
}

GLuint CreateComputeShader(const std::string& filepath) {
    std::string shaderCode;
    std::ifstream shaderFile(filepath);

    // Safe, exception-free file check
    if (!shaderFile.is_open()) {
        std::cerr << "\nCRITICAL ERROR: Could not find or open shader file at:\n" 
                  << filepath << "\n"
                  << "Ensure the file exists and the path is correct.\n\n";
        return 0;
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    shaderCode = shaderStream.str();

    const char* cShaderCode = shaderCode.c_str();
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &cShaderCode, NULL);
    glCompileShader(computeShader);

    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Compute shader compilation failed\n" << infoLog << "\n";
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR: Compute shader linking failed\n" << infoLog << "\n";
        return 0;
    }

    glDeleteShader(computeShader);
    return program;
}

GLuint CreateComputeTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    return texture;
}
