//
// Created by xu zhang on 2023-01-18.
//

#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>


using Eigen::Vector3f;
using std::string;

class Camera {
private:
    Vector3f center;
    Vector3f up;
    Vector3f lookat;

    double fov;

public:
    Camera(Vector3f center, Vector3f up, Vector3f lookat, double fov) : center(center), up(up), lookat(lookat), fov(fov) {}

    Vector3f getCenter() const {
        return center;
    }

    Vector3f getUp() const {
        return up;
    }

    Vector3f getLookat() const {
        return lookat;
    }

    double getFov() const {
        return fov;
    }

    void setCenter(Vector3f center) {
        this->center = center;
    }

    void setUp(Vector3f up) {
        this->up = up;
    }

    void setLookat(Vector3f lookat) {
        this->lookat = lookat;
    }

    void setFov(double fov) {
        this->fov = fov;
    }
};

class Output{
private:
    Camera camera;
    string filename;
    unsigned int width;
    unsigned int height;
    Vector3f backgroundColor;
    Vector3f ambientLight;

public:
    Output(Camera camera, string filename, unsigned int width, unsigned int height, Vector3f backgroundColor, Vector3f ambientLight) : camera(camera), filename(filename), width(width), height(height), backgroundColor(backgroundColor), ambientLight(ambientLight) {}

    //getters
    Camera getCamera() const {
        return camera;
    }

    string getFilename() const {
        return filename;
    }

    unsigned int getWidth() const {
        return width;
    }

    unsigned int getHeight() const {
        return height;
    }

    Vector3f getBackgroundColor() const {
        return backgroundColor;
    }

    Vector3f getAmbientLight() const {
        return ambientLight;
    }

    //setters

};


#endif //RAYTRACER_CAMERA_H
