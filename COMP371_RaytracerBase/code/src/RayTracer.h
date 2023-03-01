//
// Created by xu zhang on 2023-01-18.
//

#ifndef RAYTRACER_RAYTRACER_H
#define RAYTRACER_RAYTRACER_H


#include <vector>

#include "Geometry.h"
#include "Light.h"
#include "json.hpp"
#include "Camera.h"


using std::vector;
using std::ostream;
using json = nlohmann::json;

class RayTracer {

public:
    RayTracer(json j);

    //destructor
    ~RayTracer() {
        for (auto &i : geometryList) {
            delete i;
        }
        for (auto &i : lightList) {
            delete i;
        }
        delete output;
    }
    void run();

    void outputPPM();

private:
    vector<Geometry*> geometryList;
    vector<Light*> lightList;
    Output* output;


};


#endif //RAYTRACER_RAYTRACER_H
