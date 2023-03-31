//
// Created by xu zhang on 2023-03-23.
//

#include "Output.h"

Ray Camera::getPrimaryRay(float u, float v, int width, int height)  {
    Vector3f forward = (lookat - centre).normalized();
    Vector3f right = (forward.cross(up)).normalized();
    Vector3f upVector = up.normalized();

    // Calculate the aspect ratio and field of view angle
    float aspectRatio = static_cast<float>(width / height);
    float fovRadians = fov * M_PI / 180.0;

    // Calculate the image plane dimensions
    float halfHeight = tan(fovRadians * 0.5);
    float halfWidth = aspectRatio * halfHeight;

    // Calculate the position of the pixel on the image plane
    float s = (2.0 * u - 1.0) * halfWidth;
    float t = (1.0 - 2.0 * v) * halfHeight;

    // Calculate the ray direction
    Vector3f direction = (forward + s * right + t * upVector).normalized();

    // Create and return the primary ray
    return Ray(centre, direction);
}