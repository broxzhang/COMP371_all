//
// Created by xu zhang on 2023-01-07.
//

#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Ray.h"

using Eigen::Vector3f;
using namespace std;


class Geometry {
private:
    Vector3f ac; // ambient color
    Vector3f dc; // diffuse color
    Vector3f sc; // specular color



    double ka; // ambient coefficient
    double kd; // diffuse coefficient
    double ks; // specular coefficient

    double pc; // phong coefficient

public:
    Geometry(Vector3f ac, Vector3f dc, Vector3f sc, double ka, double kd, double ks, double pc) : ac(ac), dc(dc), sc(sc), ka(ka), kd(kd), ks(ks), pc(pc) {}

    //destructors
    virtual ~Geometry() = default;

    // output stream
    friend std::ostream& operator<<(std::ostream& os, const Geometry& geometry) {
        os << "Geometry: " << geometry.ac << " " << geometry.dc << " " << geometry.sc << " " << geometry.ka << " " << geometry.kd << " " << geometry.ks << " " << geometry.pc << std::endl;
        return os;
    }

    //inline setters and getters
    inline Vector3f getAc() const {
        return ac;
    }

    inline Vector3f getDc() const {
        return dc;
    }

    inline Vector3f getSc() const {
        return sc;
    }

    inline double getKa() const {
        return ka;
    }

    inline double getKd() const {
        return kd;
    }

    inline double getKs() const {
        return ks;
    }

    inline double getPc() const {
        return pc;
    }

    virtual bool intersect(Ray ray, float d);

    virtual string getType() = 0;
};

class Sphere : public Geometry {
private:
    Vector3f center;
    double radius;
    string type = "Sphere";
public:

    //constructors from superclass
    Sphere(Vector3f ac, Vector3f dc, Vector3f sc, double ka, double kd, double ks, double pc, Vector3f center, double radius) : Geometry(ac, dc, sc, ka, kd, ks, pc), center(center), radius(radius) {}

    //destructors
    ~Sphere() = default;

    Vector3f getCenter() const {
        return center;
    }

    double getRadius() const {
        return radius;
    }

    void setCenter(Vector3f center) {
        this->center = center;
    }

    void setRadius(double radius) {
        this->radius = radius;
    }

    Vector3f getNormal(Vector3f point) {
        return (point - center).normalized();
    }

    string getType() {
        return type;
    }

    //output stream
    friend std::ostream& operator<<(std::ostream& os, const Sphere& sphere) {
        os << "Sphere: " << sphere.center << " " << sphere.radius << static_cast<const Geometry&>(sphere) << std::endl;
        return os;
    }

    bool intersect(Ray ray, float d);
};

class Rectangle : public Geometry {
private:
    Vector3f p1;
    Vector3f p2;
    Vector3f p3;
    Vector3f p4;

    string type = "Rectangle";

public:
    //constructors
    Rectangle(Vector3f ac, Vector3f dc, Vector3f sc, double ka, double kd, double ks, double pc, Vector3f p1, Vector3f p2, Vector3f p3, Vector3f p4) : Geometry(ac, dc, sc, ka, kd, ks, pc), p1(p1), p2(p2), p3(p3), p4(p4) {}
   //destructors
    ~Rectangle() = default;

    // inline getters and setters
    inline Vector3f getP1() const { return p1; }
    inline Vector3f getP2() const { return p2; }
    inline Vector3f getP3() const { return p3; }
    inline Vector3f getP4() const { return p4; }

    inline void setP1(Vector3f p1) { this->p1 = p1; }
    inline void setP2(Vector3f p2) { this->p2 = p2; }
    inline void setP3(Vector3f p3) { this->p3 = p3; }
    inline void setP4(Vector3f p4) { this->p4 = p4; }

    string getType() {
        return type;
    }

};



#endif //RAYTRACER_GEOMETRY_H
