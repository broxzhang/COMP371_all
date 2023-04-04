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

Vector3f PointLight::computeContribution(const Ray& ray, const HitRecord& hitRecord) const {
    Vector3f lightDirection = (center - hitRecord.p).normalized();
    float distanceSquared = (center - hitRecord.p).squaredNorm();

    // Compute diffuse contribution
    float kd = hitRecord.material->kd;
    Vector3f dc = hitRecord.material->dc;
    float nDotL = std::max(0.0f, hitRecord.n.dot(lightDirection));
    Vector3f diffuseContribution = kd * (dc.cwiseProduct(is)) * nDotL / distanceSquared;

    // Compute specular contribution (using Phong model)
    float ks = hitRecord.material->ks;
    Vector3f sc = hitRecord.material->sc;
    int n = hitRecord.material->pc;
    Vector3f viewDirection = (ray.getOrigin() - hitRecord.p).normalized();
    Vector3f reflectionDirection = (-lightDirection + 2.0f * hitRecord.n * nDotL).normalized();
    float rDotV = std::max(0.0f, reflectionDirection.dot(viewDirection));
    Vector3f specularContribution = ks * (sc.cwiseProduct(is)) * std::pow(rDotV, n) / distanceSquared;

    return diffuseContribution + specularContribution;
}

Vector3f AreaLight::computeContribution(const Ray& ray, const HitRecord& hitRecord) const {
    // Sample a point on the area light
    float u = static_cast<float>(rand()) / RAND_MAX;
    float v = static_cast<float>(rand()) / RAND_MAX;
    Vector3f sampledPoint = p1 + u * (p2 - p1) + v * (p3 - p1);

    // Compute light direction and distance squared
    Vector3f lightDirection = (sampledPoint - hitRecord.p).normalized();
    float distanceSquared = (sampledPoint - hitRecord.p).squaredNorm();

    // Compute diffuse contribution
    float kd = hitRecord.material->kd;
    Vector3f dc = hitRecord.material->dc;
    float nDotL = std::max(0.0f, hitRecord.n.dot(lightDirection));
    Vector3f diffuseContribution = kd * (dc.cwiseProduct(id)) * nDotL / distanceSquared;

    // Compute specular contribution (using Phong model)
    float ks = hitRecord.material->ks;
    Vector3f sc = hitRecord.material->sc;
    int n = hitRecord.material->pc;
    Vector3f viewDirection = (ray.getOrigin() - hitRecord.p).normalized();
    Vector3f reflectionDirection = (-lightDirection + 2.0f * hitRecord.n * nDotL).normalized();
    float rDotV = std::max(0.0f, reflectionDirection.dot(viewDirection));
    Vector3f specularContribution = ks * (sc.cwiseProduct(id)) * std::pow(rDotV, n) / distanceSquared;

    return diffuseContribution + specularContribution;
}