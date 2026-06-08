#include <glad/glad.h> 
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "physics.hpp"
#include "renderer.hpp"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

int main() {
    GLFWwindow* window = InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Atomos");
    if (!window) return -1;
    GLuint computeProgram = CreateComputeShader("shader.comp");
    if (computeProgram == 0) {
        std::cerr << "Terminating because compute shader failed to load.\n";
        glfwTerminate();
        return -1;
    }


    GLuint outputTexture = CreateComputeTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    GLuint tempFBO;
glGenFramebuffers(1, &tempFBO);

glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFBO);

glFramebufferTexture2D(
    GL_READ_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,
    outputTexture,
    0
);

glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    
    State cameraState;
    
    Camera camera;
    camera.position = Eigen::Vector3d(0.0, 0.0, EARTH_RADIUS + 1.0);

    //fetch uniform locations
    GLint camPosLoc   = glGetUniformLocation(computeProgram, "camPos");
    GLint camDirLoc   = glGetUniformLocation(computeProgram, "camDir");
    GLint camUpLoc    = glGetUniformLocation(computeProgram, "camUp");
    GLint camRightLoc = glGetUniformLocation(computeProgram, "camRight");
    GLint sunDirLoc   = glGetUniformLocation(computeProgram, "sunDir");

    double time = 0.0;
    double lastTime = glfwGetTime();
    int frameCount = 0;


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // MAIN Render Loop
    while (!glfwWindowShouldClose(window)) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (camera.firstMouse)
        {
            camera.lastMouseX = xpos;
            camera.lastMouseY = ypos;
            camera.firstMouse = false;
        }

        double xoffset = camera.lastMouseX - xpos;
        double yoffset = camera.lastMouseY - ypos;

        camera.lastMouseX = xpos;
        camera.lastMouseY = ypos;

        camera.processMouseMovement(xoffset, yoffset);

        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        const double moveSpeed = 100000.0;   //change

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.position += camera.forward * moveSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.position -= camera.forward * moveSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.position -= camera.right * moveSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.position += camera.right * moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.position += Eigen::Vector3d(0.0, 0.0, 1.0) * moveSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.position -= Eigen::Vector3d(0.0, 0.0, 1.0) * moveSpeed * deltaTime;
        // Animate sun from Zenith to Horizon
        time += 0.005;
        double sunAngle = time; // Radians
        Eigen::Vector3d sunDir(std::cos(sunAngle), 0.0, std::sin(sunAngle));
        sunDir.normalize();

        glUseProgram(computeProgram);
        glUniform3f(sunDirLoc, sunDir.x(), sunDir.y(), sunDir.z() );

        // Explicit Eigen indexing extraction to prevent GLSL uniform packing issues
        glUniform3f(camPosLoc,camera.position.x(),camera.position.y(),camera.position.z());
        glUniform3f(camDirLoc,camera.forward.x(),camera.forward.y(),camera.forward.z());
        glUniform3f(camUpLoc,camera.up.x(),camera.up.y(),camera.up.z());
        glUniform3f(camRightLoc,camera.right.x(),camera.right.y(),camera.right.z());

        //std::cout << "[DEBUG] Dispatching Compute (Spam Warning)...\n";
        double t0 = glfwGetTime();
        glUniform1i(glGetUniformLocation(computeProgram, "frameCount"), frameCount);
        // 8x8 work groups
        glDispatchCompute((WINDOW_WIDTH + 7) / 8, (WINDOW_HEIGHT + 7) / 8, 1);
        
        // Ensure image is written before blitting
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        frameCount++;

//------------fps
        static double lastFPSTime = glfwGetTime();
        static int frames = 0;
        frames++;
        double now = glfwGetTime();
        if (now - lastFPSTime >= 1.0)
        {
            std::cout << "FPS: " << frames << "\n";
            frames = 0;
            lastFPSTime = now;
        }
//double t1 = glfwGetTime();
// std::cout << "GPU frame = "
//           << (t1 - t0) * 1000.0
//           << " ms\n";
        // Blit compute texture directly to the default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
        
        // Setup a temporary FBO just to blit the texture
        glBindFramebuffer(GL_READ_FRAMEBUFFER, tempFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(
            0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
            0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &tempFBO);
    return 0;
}