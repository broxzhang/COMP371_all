//
// Created by xu zhang on 2023-01-18.
//



#include <fstream>
#include "RayTracer.h"
#include "Ray.h"

using namespace std;


RayTracer::RayTracer(json j) {
    json geometries = j["geometry"];

    cout << geometries << endl;

    for (auto &geometry : geometries) {
        if (geometry["type"] == "sphere") {
            Vector3f center = Vector3f(geometry["centre"][0], geometry["centre"][1], geometry["centre"][2]);
            double radius = geometry["radius"];

            Vector3f ac = Vector3f(geometry["ac"][0], geometry["ac"][1], geometry["ac"][2]);
            Vector3f dc = Vector3f(geometry["dc"][0], geometry["dc"][1], geometry["dc"][2]);
            Vector3f sc = Vector3f(geometry["sc"][0], geometry["sc"][1], geometry["sc"][2]);

            double ka = geometry["ka"];
            double kd = geometry["kd"];
            double ks = geometry["ks"];

            double pc = geometry["pc"];

            geometryList.push_back(new Sphere(ac, dc, sc, ka, kd, ks, pc, center, radius));
        }
    }

    json lights = j["light"];
    for (auto &light : lights) {
        if (light["type"] == "point") {
            Vector3f center = Vector3f(light["centre"][0], light["centre"][1], light["centre"][2]);
            Vector3f id = Vector3f(light["id"][0], light["id"][1], light["id"][2]);
            Vector3f is = Vector3f(light["is"][0], light["is"][1], light["is"][2]);

            lightList.push_back(new Point(center, id, is));
        }
    }

    json outputJson = j["output"];

    Vector3f center = Vector3f(outputJson[0]["centre"][0], outputJson[0]["centre"][1], outputJson[0]["centre"][2]);
    Vector3f up = Vector3f(outputJson[0]["up"][0], outputJson[0]["up"][1], outputJson[0]["up"][2]);
    Vector3f lookAt = Vector3f(outputJson[0]["lookat"][0], outputJson[0]["lookat"][1], outputJson[0]["lookat"][2]);
    string filename = outputJson[0]["filename"];
    double fov = outputJson[0]["fov"];
    unsigned int width = outputJson[0]["size"][0];
    unsigned int height = outputJson[0]["size"][1];

    Vector3f backgroundColor = Vector3f(outputJson[0]["bkc"][0], outputJson[0]["bkc"][1], outputJson[0]["bkc"][2]);
    Vector3f ambientLight = Vector3f(outputJson[0]["ai"][0], outputJson[0]["ai"][1], outputJson[0]["ai"][2]);

    Camera* ca = new Camera(center, up, lookAt, fov);
    output = new Output(*ca, filename, width, height, backgroundColor, ambientLight);

}

void RayTracer::run() {
    cout << "RayTracer::run()" << endl;
//    for (auto &go : geometryList) {
//        cout << go << endl;
//    }
}

