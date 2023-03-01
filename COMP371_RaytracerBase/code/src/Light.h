//
// Created by xu zhang on 2023-01-07.
//

#ifndef RAYTRACER_LIGHT_H
#define RAYTRACER_LIGHT_H

#include <Eigen/Core>
#include <Eigen/Dense>

using Eigen::Vector3f;
using std::string;

class Light {
private:
    string type;
    Vector3f center;
    Vector3f id; //diffuse
    Vector3f is; //specular

public:
    //default constructor
    Light() : type("light") {}
    //constructor with type
    Light(string type) : type(type) {}
    //constructor with type, center, id, is
    Light(string type, Vector3f center, Vector3f id, Vector3f is) : type(type), center(center), id(id), is(is) {}
    Light(Vector3f center, Vector3f id, Vector3f is) : center(center), id(id), is(is) {}

    //getter and setter
    Vector3f getCenter() const {
        return center;
    }

    Vector3f getId() const {
        return id;
    }

    Vector3f getIs() const {
        return is;
    }

    void setCenter(Vector3f center) {
        this->center = center;
    }

};

class Point: public Light {
public:
    //default constructor
    Point() : Light("point") {}
    //constructor with all parameters
    Point(Vector3f center, Vector3f id, Vector3f is) : Light("point", center, id, is) {}

    //destructors
    ~Point() = default;


};

class AreaLight: public Light {
private: Vector3f P1, P2, P3, P4;
public:
    //default constructor
    AreaLight() : Light("area") {}
    //constructor with all parameters
    AreaLight(Vector3f center, Vector3f id, Vector3f is, Vector3f P1, Vector3f P2, Vector3f P3, Vector3f P4) : Light("area", center, id, is), P1(P1), P2(P2), P3(P3), P4(P4) {}

    //destructors
    ~AreaLight() = default;
};




#endif //RAYTRACER_LIGHT_H
