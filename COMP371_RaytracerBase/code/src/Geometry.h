//
// Created by xu zhang on 2023-01-07.
//

#ifndef RAYTRACER_GEOMETRY_H
#define RAYTRACER_GEOMETRY_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "HitRecord.h"

using Eigen::Vector3f;
using namespace std;

class Material {
public:
    Vector3f ac; // ambient color
    Vector3f dc; // diffuse color
    Vector3f sc; // specular color

    float ka; // ambient coefficient
    float kd; // diffuse coefficient
    float ks; // specular coefficient
    float pc; // phong coefficient

    //default constructor
    Material() : ac(Vector3f(0, 0, 0)), dc(Vector3f(0, 0, 0)), sc(Vector3f(0, 0, 0)), ka(0), kd(0), ks(0), pc(0) {}

    //copy constructor
    Material(const Material& material) : ac(material.ac), dc(material.dc), sc(material.sc), ka(material.ka), kd(material.kd), ks(material.ks), pc(material.pc) {}

    //constructor
    Material(Vector3f ac, Vector3f dc, Vector3f sc, float ka, float kd, float ks, float pc) : ac(ac), dc(dc), sc(sc), ka(ka), kd(kd), ks(ks), pc(pc) {}

    //operator=
    Material& operator=(const Material& material) {
        ac = material.ac;
        dc = material.dc;
        sc = material.sc;
        ka = material.ka;
        kd = material.kd;
        ks = material.ks;
        pc = material.pc;
        return *this;
    }

    //output
    friend ostream& operator<<(ostream& os, const Material& material) {
        os << "ac: " << material.ac << endl;
        os << "dc: " << material.dc << endl;
        os << "sc: " << material.sc << endl;
        os << "ka: " << material.ka << endl;
        os << "kd: " << material.kd << endl;
        os << "ks: " << material.ks << endl;
        os << "pc: " << material.pc << endl;
        return os;
    }

    //destructor
    ~Material() {}

    Vector3f evalBRDF(const HitRecord& hitRecord, const Vector3f& viewDirection, const Vector3f& lightDirection, const Vector3f& halfVector);

};
class Surface {
public:
    Material material;
    Vector3f center;

    //default constructor
    Surface() : material(Material()), center(0) {}

    //copy constructor
    Surface(const Surface& surface) : material(surface.material), center(surface.center) {}

    //constructor
    Surface(Material material, Vector3f center) : material(material), center(center) {}

    //operator=
    Surface& operator=(const Surface& surface) {
        material = surface.material;
        center = surface.center;
        return *this;
    }

    //output
    friend ostream& operator<<(ostream& os, const Surface& surface) {
        os << "material: " << surface.material << endl;
        os << "center: " << surface.center << endl;
        return os;
    }

    //destructor
    ~Surface() {}

};

class Geometry {
public:
    string type;
    Surface surface;
    bool visible = true;

    //default constructor
    Geometry() : type("default"), surface(Surface()) {}

    //copy constructor
    Geometry(const Geometry& geometry) : type(geometry.type), surface(geometry.surface) {}

    //constructor
    Geometry(string type, Surface surface) : type(type), surface(surface) {}

    //operator=
    Geometry& operator=(const Geometry& geometry) {
        type = geometry.type;
        surface = geometry.surface;
        return *this;
    }

    //destructor
    virtual ~Geometry() {}

    virtual HitRecord intersect(const Ray& ray) const = 0;

    virtual bool hit(const Ray& ray, float t0, float closestHitDistance, HitRecord& currentHitRecord) = 0;


    //getter and setter
    bool isVisible() const {
        return visible;
    }

    void setVisible(bool visible) {
        Geometry::visible = visible;
    }

    const string &getType() const {
        return type;
    }

    void setType(const string &type) {
        Geometry::type = type;
    }

    const Surface &getSurface() const {
        return surface;
    }

    void setSurface(const Surface &surface) {
        Geometry::surface = surface;
    }
};

class Sphere : public Geometry {
private:
    float radius;

public:
    //default constructor
    Sphere() : Geometry(), radius(0) {}

    //copy constructor
    Sphere(const Sphere& sphere) : Geometry(sphere), radius(sphere.radius) {}

    //constructor
    Sphere( Surface surface, float radius) : Geometry("Sphere", surface), radius(radius) {}

    //operator=
    Sphere& operator=(const Sphere& sphere) {
        Geometry::operator=(sphere);
        radius = sphere.radius;
        return *this;
    }

    //output
    friend ostream& operator<<(ostream& os, const Sphere& sphere) {
        os << "type: " << sphere.getType() << endl;
        os << "surface: " << sphere.getSurface() << endl;
        os << "radius: " << sphere.radius << endl;
        return os;
    }
    //destructor
    ~Sphere() {}

    virtual HitRecord intersect(const Ray& ray) const;

    bool hit(const Ray& ray, float t0, float closestHitDistance, HitRecord& currentHitRecord) override;
};

class Rectangle : public Geometry {
private:
    Vector3f p1, p2, p3, p4;

public:
    //default constructor
    Rectangle() : Geometry(), p1(Vector3f(0, 0, 0)), p2(Vector3f(0, 0, 0)), p3(Vector3f(0, 0, 0)), p4(Vector3f(0, 0, 0)) {}

    //copy constructor
    Rectangle(const Rectangle& rectangle) : Geometry(rectangle), p1(rectangle.p1), p2(rectangle.p2), p3(rectangle.p3), p4(rectangle.p4) {}

    //constructor
    Rectangle(Surface surface, Vector3f p1, Vector3f p2, Vector3f p3, Vector3f p4) : Geometry("Rectangle", surface), p1(p1), p2(p2), p3(p3), p4(p4) {}

    //operator=
    Rectangle& operator=(const Rectangle& rectangle) {
        Geometry::operator=(rectangle);
        p1 = rectangle.p1;
        p2 = rectangle.p2;
        p3 = rectangle.p3;
        p4 = rectangle.p4;
        return *this;
    }

    //destructor
    ~Rectangle() {}

    virtual HitRecord intersect(const Ray& ray) const override;

    bool hit(const Ray& ray, float t0, float closestHitDistance, HitRecord& currentHitRecord) override;
};



#endif //RAYTRACER_GEOMETRY_H
