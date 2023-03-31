//
// Created by xu zhang on 2023-01-18.
//

#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H


#include <vector>

#include "Geometry.h"
#include "json.hpp"
#include "Light.h"
#include "Output.h"


using std::vector;
using std::ostream;
using json = nlohmann::json;

class RayTracer {

public:
    RayTracer(json j);

    Vector3f getDiffuseReflection(HitRecord& hit, Ray& ray, Vector3f vec);
    Vector3f getSpecularReflectance(HitRecord& hrec, Ray& ray, Vector3f& n);
    Vector3f getRandomVector(Ray& ray, Vector3f hitPoint, Vector3f n);
    Vector3f getDirectIllumination(const HitRecord& hitRecord, const Vector3f& hitPoint,  const Ray& ray);
    Vector3f getRandomDirectionInHemisphere(const Vector3f& normal);
    Vector3f radiance(Ray& ray, int depth);
    HitRecord intersect(Ray& ray);

    void run();


    vector<Geometry*> geometryList;
    vector<Light*> lightList;
    Output output;

    void computeShading(const Ray &ray, HitRecord &hitRecord);
    bool groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn);

    Vector3f computeAmbientColor(const HitRecord &hitRecord);
};


#endif //RAYTRACER_RAYTRACER_H
