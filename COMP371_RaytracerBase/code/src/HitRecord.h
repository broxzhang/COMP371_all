//
// Created by xu zhang on 2023-02-02.
//

#ifndef RAYTRACER_HITRECORD_H
#define RAYTRACER_HITRECORD_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Ray.h"
#include "Surface.h"

using Eigen::Vector3f;


class Material {
private:
    Vector3f l; //light
    Vector3f v; //view
    Vector3f n; //normal

    Vector3f ac; //ambient color
    Vector3f dc; //diffuse color
    Vector3f sc; //specular color

    float ka; //ambient coefficient
    float kd; //diffuse coefficient
    float ks; //specular coefficient
    float pc; //phong coefficient

public:
    //default constructor
    Material() : l(Vector3f(0, 0, 0)), v(Vector3f(0, 0, 0)), n(Vector3f(0, 0, 0)), ac(Vector3f(0, 0, 0)), dc(Vector3f(0, 0, 0)), sc(Vector3f(0, 0, 0)), ka(0), kd(0), ks(0), pc(0) {}

    //copy constructor
    Material(const Material& material) : l(material.l), v(material.v), n(material.n), ac(material.ac), dc(material.dc), sc(material.sc), ka(material.ka), kd(material.kd), ks(material.ks), pc(material.pc) {}

    //operator=
    Material& operator=(const Material& material) {
        l = material.l;
        v = material.v;
        n = material.n;
        ac = material.ac;
        dc = material.dc;
        sc = material.sc;
        ka = material.ka;
        kd = material.kd;
        ks = material.ks;
        pc = material.pc;
        return *this;
    }

};

class HitRecord {
private:
    float t;
    Surface* surface;
    Vector3f n; //normal
    Vector3f color; //color
public:
    //constructor with only t
    HitRecord(float t) : t(t), surface(nullptr), n(Vector3f(0, 0, 0)), color(Vector3f(0, 0, 0)) {}
    //default constructor
    HitRecord() : HitRecord(INFINITY) {}
    //constructor with t, surface, n, color
    HitRecord(float t, Surface* surface, Vector3f n, Vector3f color) : t(t), surface(surface), n(n), color(color) {}

    //copy constructor
    HitRecord(const HitRecord& hitRecord) : t(hitRecord.t), surface(hitRecord.surface), n(hitRecord.n), color(hitRecord.color) {}

    //operator=
    HitRecord& operator=(const HitRecord& hitRecord) {
        t = hitRecord.t;
        surface = hitRecord.surface;
        n = hitRecord.n;
        color = hitRecord.color;
        return *this;
    }

    //destructors
    ~HitRecord() = default;

    //getters
    float getT() const {
        return t;
    }

    Surface* getSurface() const {
        return surface;
    }

    Vector3f getN() const {
        return n;
    }

    Vector3f getColor() const {
        return color;
    }

    //setters
    void setT(float t) {
        this->t = t;
    }

    void setSurface(Surface* surface) {
        this->surface = surface;
    }

    void setN(Vector3f n) {
        this->n = n;
    }

    void setColor(Vector3f color) {
        this->color = color;
    }

};

#endif //RAYTRACER_HITRECORD_H
