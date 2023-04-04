//
// Created by xu zhang on 2023-01-18.
//

#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H

#pragma once

#include <vector>

#include "Geometry.h"
#include "json.hpp"
#include "Light.h"
#include "Output.h"
#include "HitRecord.h"

using std::vector;
using std::ostream;
using json = nlohmann::json;

class RayTracer {

public:
    RayTracer(json j);

    //destructor
    ~RayTracer();
    Vector3f getAmbientReflectance(Vector3f ac, float ka);
    Vector3f getDiffuseReflection(HitRecord& hit, Ray& ray);
    Vector3f getSpecularReflectance(HitRecord& hrec, Ray& ray);
    Vector3f getRandomVector(Ray& ray, Vector3f hitPoint, Vector3f n);
    Vector3f getDirectIllumination(const HitRecord& hitRecord, const Vector3f& hitPoint,  const Ray& ray);
    Vector3f getRandomDirectionInHemisphere(const Vector3f& normal);
    Vector3f radiance(HitRecord& currentHit, Vector3f o, Vector3f d);

    void run();


    vector<Geometry*> geometryList;
    vector<Light*> lightList;
    Output output;

    Vector3f computeShading(Ray& ray, HitRecord& hit);
    bool groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn);

    Vector3f computeAmbientColor(const HitRecord &hitRecord);

    bool groupRaycastHit(const Ray &ray, float t0, float t1, HitRecord &hitRecord);

    Vector3f computeAmbientLight(const HitRecord& hitRecord);

    void handleAntialiasing(const Ray& ray, HitRecord& hitRecord);

    Ray generateStratifiedJitteredRay(const Ray& originalRay, int a, int b, int sampleIndex);

    Ray generateJitteredRay(const Ray& ray);
    Vector3f computeGlobalIlluminationContribution(const Ray& ray, const HitRecord& hitRecord);
    float computePixelSizeDeltaHorizontal(float horizontal_fov, float height);
    float computePixelSizeDeltaVertical(float horizontal_fov, float height);
    int save_ppm(const vector<float> &buffer, int dimx, int dimy);
};


#endif //RAYTRACER_RAYTRACER_H
