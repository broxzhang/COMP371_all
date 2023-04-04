//
// Created by xu zhang on 2023-03-23.
//

#ifndef RAYTRACER_OUTPUT_H
#define RAYTRACER_OUTPUT_H

#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>
#include "HitRecord.h"

using Eigen::Vector3f;
using Eigen::Vector2i;
using namespace std;
using std::vector;

class Camera {
private:
    Vector3f lookat;
    Vector3f up;
    float fov;
    Vector3f centre;

public:
    //default constructor
    Camera() : lookat(Vector3f(0, 0, 0)), up(Vector3f(0, 0, 0)), fov(0), centre(Vector3f(0, 0, 0)) {}

    //constructor with parameters
    Camera(Vector3f lookat, Vector3f up, float fov, Vector3f centre) : lookat(lookat), up(up), fov(fov), centre(centre) {}

    //copy constructor
    Camera(const Camera& camera) : lookat(camera.lookat), up(camera.up), fov(camera.fov), centre(camera.centre) {}

    //Getters and Setters
    Vector3f getLookat() const { return lookat; }
    Vector3f getUp() const { return up; }

    float getFov() const { return fov; }
    Vector3f getCentre() const { return centre; }

    void setLookat(Vector3f lookat) { this->lookat = lookat; }
    void setUp(Vector3f up) { this->up = up; }

    void setFov(float fov) { this->fov = fov; }
    void setCentre(Vector3f centre) { this->centre = centre; }

    //destructor
    ~Camera() {}

    Ray getPrimaryRay(float u, float v, int width, int height);
};

class Output {
public:
    string filename;
    Vector2i size;
    Camera camera;

    Vector3f ai; // ambient intensity
    Vector3f bkc; // background color

    bool antialiasing = false; // whether use antialiasing
    bool globalillum = false; // whether use global illumination

    vector<int> raysPerPixel = {10,10}; // number of rays per pixel

    float maxbounces = 0; // maximum number of bounces
    float probterminate = 0.1; // probability of terminating a ray

    // default constructor
    Output() : filename(""), size(Vector2i(0, 0)), camera(Camera()), ai(Vector3f(0, 0, 0)), bkc(Vector3f(0, 0, 0)) {}

    // constructor with parameters
    Output(string filename, Vector2i size, Camera camera, Vector3f ai, Vector3f bkc) :
            filename(filename), size(size), camera(camera), ai(ai), bkc(bkc) {}
    // gettters for antialiasing, globalillum, maxbounces, probterminate and raysPerPixel
    bool getAntialiasing() const { return antialiasing; }
    bool getGlobalillum() const { return globalillum; }
    float getMaxbounces() const { return maxbounces; }
    float getProbterminate() const { return probterminate; }
    vector<int> getRaysPerPixel() const { return raysPerPixel; }

    // setters for antialiasing, globalillum, maxbounces, probterminate and raysPerPixel
    void setAntialiasing(bool antialiasing) { this->antialiasing = antialiasing; }
    void setGlobalillum(bool globalillum) { this->globalillum = globalillum; }
    void setMaxbounces(float maxbounces) { this->maxbounces = maxbounces; }
    void setProbterminate(float probterminate) { this->probterminate = probterminate; }
    void setRaysPerPixel(vector<int> raysPerPixel) { this->raysPerPixel = raysPerPixel; }

    // getters and setters
    string getFilename() const { return filename; }
    Vector2i getSize() const { return size; }

    Camera getCamera() const { return camera; }
    Vector3f getAi() const { return ai; }

    Vector3f getBkc() const { return bkc; }

    void setFilename(string filename) { this->filename = filename; }
    void setSize(Vector2i size) { this->size = size; }

    void setCamera(Camera camera) { this->camera = camera; }
    void setAi(Vector3f ai) { this->ai = ai; }

    void setBkc(Vector3f bkc) { this->bkc = bkc; }

    // destructor
    ~Output() {}
};


#endif //RAYTRACER_OUTPUT_H
