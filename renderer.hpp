#pragma once
#include <string>
#include <Eigen/Dense>

struct GLFWwindow;
typedef unsigned int GLuint;

GLFWwindow* InitWindow(int width, int height, const char* title);
GLuint CreateComputeShader(const std::string& filepath);
GLuint CreateComputeTexture(int width, int height);

struct Camera {
    Eigen::Vector3d position{0.0, 0.0, 0.0};
    Eigen::Vector3d forward{1.0, 0.0, 0.0};
    Eigen::Vector3d right{0.0, 1.0, 0.0};
    Eigen::Vector3d up{0.0, 0.0, 1.0};

    double yaw = 0.0;
    double pitch = 0.0;

    double lastMouseX = 640.0;
    double lastMouseY = 360.0;
    bool firstMouse = true;

    double mouseSensitivity = 0.1;

    void processMouseMovement(double xoffset, double yoffset)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        pitch = std::clamp(pitch, -89.0, 89.0);

        Eigen::Vector3d front;

        double yawRad   = yaw   * M_PI / 180.0;
        double pitchRad = pitch * M_PI / 180.0;

        front.x() = std::cos(yawRad) * std::cos(pitchRad);
        front.y() = std::sin(yawRad) * std::cos(pitchRad);
        front.z() = std::sin(pitchRad);

        forward = front.normalized();

        Eigen::Vector3d worldUp(0.0, 0.0, 1.0);

        right = forward.cross(worldUp).normalized();
        up    = right.cross(forward).normalized();
    }
};