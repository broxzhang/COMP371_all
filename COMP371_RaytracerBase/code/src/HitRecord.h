//
// Created by xu zhang on 2023-03-23.
//

#ifndef RAYTRACER_HITRECORD_H
#define RAYTRACER_HITRECORD_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Geometry.h"

using Eigen::Vector3f;
using namespace std;

class Ray {
public:
    Vector3f origin;
    Vector3f direction;


    //default constructor
    Ray() : origin(Vector3f(0, 0, 0)), direction(Vector3f(0, 0, 0)) {}

    //copy constructor
    Ray(const Ray& ray) : origin(ray.origin), direction(ray.direction) {}

    //constructor
    Ray(Vector3f origin, Vector3f direction) : origin(origin), direction(direction) {}

    //operator=
    Ray& operator=(const Ray& ray) {
        origin = ray.origin;
        direction = ray.direction;
        return *this;
    }

    //destructor
    ~Ray() {}

    //getter
    Vector3f getOrigin() const { return origin; }
    Vector3f getDirection() const { return direction; }

    //setter
    void setOrigin(Vector3f origin) { this->origin = origin; }
    void setDirection(Vector3f direction) { this->direction = direction; }

    Vector3f evaluate(float t) const;

};



class HitRecord {
public:
    Vector3f p, n; // p: point of intersection, n: normal at intersection
    float t; // t: distance from ray origin to intersection point
    bool hit; // hit: whether the ray hits the object
    Material* material; // material: material of the object

public:
    // default constructor
    HitRecord() : p(Vector3f(0, 0, 0)), n(Vector3f(0, 0, 0)), t(0), hit(false), material(nullptr) {}

    // constructor with all parameters
    HitRecord(Vector3f p, Vector3f n, float t, bool hit, Material* material) : p(p), n(n), t(t), hit(hit), material(material) {}

    // copy constructor
    HitRecord(const HitRecord& hitRecord) : p(hitRecord.p), n(hitRecord.n), t(hitRecord.t), hit(hitRecord.hit), material(hitRecord.material) {}

    // operator=
    HitRecord& operator=(const HitRecord& hitRecord) {
        p = hitRecord.p;
        n = hitRecord.n;
        t = hitRecord.t;
        hit = hitRecord.hit;
        material = hitRecord.material;
        return *this;
    }

    // destructor
    ~HitRecord() {}


};


#endif //RAYTRACER_HITRECORD_H
