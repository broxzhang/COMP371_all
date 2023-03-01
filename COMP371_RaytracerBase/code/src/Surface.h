//
// Created by xu zhang on 2023-02-02.
//

#ifndef RAYTRACER_SURFACE_H
#define RAYTRACER_SURFACE_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "HitRecord.h"

using Eigen::Vector3f;

class Surface {
private:
    Vector3f center;
    Material* material;
public:
    //default constructor
    Surface() : center(Vector3f(0, 0, 0)), material(nullptr) {}

    //copy constructor
    Surface(const Surface& surface) : center(surface.center), material(surface.material) {}

    //operator=
    Surface& operator=(const Surface& surface) {
        center = surface.center;
        material = surface.material;
        return *this;
    }

    //destructors
    ~Surface() = default;

    //getters
    Vector3f getCenter() const {
        return center;
    }

    Material* getMaterial() const {
        return material;
    }

    //setters
    void setCenter(Vector3f center) {
        this->center = center;
    }

    void setMaterial(Material* material) {
        this->material = material;
    }

    void Info() {}

    bool hit(Ray& ray, float t0, float t1, HitRecord& hitRecord) {
    }
};



#endif //RAYTRACER_SURFACE_H
