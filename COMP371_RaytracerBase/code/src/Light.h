//
// Created by xu zhang on 2023-03-23.
//

#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H

#include <Eigen/Core>
#include <Eigen/Dense>

using Eigen::Vector3f;
using namespace std;

class Light {
public:
    string type;
    Vector3f id; // intensity of diffuse
    Vector3f is; // intensity of specular

    bool use = true; // whether use this light

    // constructor with parameters
    Light(string type, Vector3f id, Vector3f is) : type(type), id(id), is(is) {}

    virtual Vector3f getDirection(const Vector3f& hitPoint) const = 0;
    virtual float getDistance(const Vector3f& hitPoint) const = 0;
    virtual Vector3f getIntensity(const Vector3f& hitPoint) const = 0;
};

class AreaLight : public Light {
public:
    Vector3f p1, p2, p3, p4; // four points of the area light
    int n;
    bool useCenter = true;

    // Constructor with parameters
    AreaLight(Vector3f id, Vector3f is, Vector3f p1, Vector3f p2, Vector3f p3, Vector3f p4, int n) : Light("Area",id, is), p1(p1), p2(p2), p3(p3), p4(p4), n(n) {}

    // getter and setter for useCenter
    bool getUseCenter() const { return useCenter; }
    void setUseCenter(bool useCenter) { this->useCenter = useCenter; }

    // getter and setter for n
    int getN() const { return n; }
    void setN(int n) { this->n = n; }

    Vector3f getDirection(const Vector3f& hitPoint) const override {
        Vector3f u = p2 - p1;
        Vector3f v = p4 - p1;
        // Sample a point on the area light
        Vector3f samplePoint = p1 + u * drand48() + v * drand48();
        return (samplePoint - hitPoint).normalized();
    }

    float getDistance(const Vector3f& hitPoint) const override {
        Vector3f u = p2 - p1;
        Vector3f v = p4 - p1;
        Vector3f samplePoint = p1 + u * drand48() + v * drand48();
        return (samplePoint - hitPoint).norm();
    }

    Vector3f getIntensity(const Vector3f& hitPoint) const override {
        float distance = getDistance(hitPoint);
        return is / (distance * distance);
    }
};

class PointLight : public Light {
public:
    Vector3f center; // center of the point light

    // Constructor with parameters
    PointLight(Vector3f id, Vector3f is, Vector3f center) : Light("Point",id, is), center(center) {}

    Vector3f getRadianceAt(const Vector3f& point);

    Vector3f getDirection(const Vector3f& hitPoint) const override {
        return (center - hitPoint).normalized();
    }

    float getDistance(const Vector3f& hitPoint) const override {
        return (center - hitPoint).norm();
    }

    Vector3f getIntensity(const Vector3f& hitPoint) const override {
        // You can adjust the intensity calculation as needed, e.g., by using falloff or other factors
        float distance = getDistance(hitPoint);
        return center / (distance * distance);
    }
};


#endif //RAYTRACER_LIGHT_H
