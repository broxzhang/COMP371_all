//
// Created by xu zhang on 2023-01-18.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <Eigen/Core>
#include <Eigen/Dense>

using Eigen::Vector3f;

class Ray {
private:
    Vector3f origin;
    Vector3f direction;

public:
    //default constructor
    Ray() = default;
    //constructor
    Ray(Vector3f origin, Vector3f direction) : origin(origin), direction(direction) {}
    //destructors
    ~Ray() = default;
    //copy constructor
    Ray(const Ray &ray) : origin(ray.origin), direction(ray.direction) {}

    //inline setters and getters
    inline Vector3f getOrigin() const {
        return origin;
    }

    inline Vector3f getDirection() const {
        return direction;
    }

    inline void setOrigin(Vector3f origin) {
        this->origin = origin;
    }

    inline void setDirection(Vector3f direction) {
        this->direction = direction;
    }

    inline Vector3f at(double t) const {
        return origin + t * direction;
    }





};



#endif //RAYTRACER_RAY_H
