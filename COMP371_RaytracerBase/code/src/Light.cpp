//
// Created by xu zhang on 2023-03-23.
//

#include "Light.h"

Vector3f PointLight::getRadianceAt(const Vector3f& point) {
    // Calculate the distance between the light source and the point
    float distance = (center - point).norm();

    // Calculate the attenuation factor based on the distance
    float attenuation = 1.0f / (1.0f + 0.01f * distance + 0.001f * distance * distance);

    // Return the radiance at the point
    return is * attenuation;
}
