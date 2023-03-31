//
// Created by xu zhang on 2023-01-18.
//


#include <iostream>

#include <fstream>
#include "RayTracer.h"

using namespace std;

bool local_illumination = false; // local illumination or global illumination
bool shadow = false; // shadow or not
bool antialiasing = false; // antialiasing or not

const float MIN_RAY_LENGTH = 0.0001f; // minimum ray length
const float MAX_RAY_LENGTH = 100000.0f; // maximum ray length

const int NB_SAMPLES_PER_PIXEL = 10; // number of samples per pixel
const int MAX_BOUNCES = 5; // number of bounces

int current_path_sample = 0; // current path sample
int current_path_bounce = 0; // current path bounce
bool stratified_sampling = false; // stratified sampling or not

const unsigned int cells_x = 10; // number of cells in x direction
const unsigned int cells_y = 10; // number of cells in y direction
const unsigned int cells_size = NB_SAMPLES_PER_PIXEL / (cells_x * cells_y); // number of samples per cell

float p_termination_threshold = 0.1f; // probability of termination threshold
float p = 0.0f; // probability of termination
float p_reflection = 0.1f; // probability of reflection

float cos_theta; // cosine of theta
const Vector3f light_color(0, 0, 0); // light color

// Li and Le
const Vector3f Li(0, 0, 0);
const Vector3f Le(0, 0, 0);

double getMonteCarloEstimate(int n) {
    double x, sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += exp(sin(3 * pow(drand48(), 2)));
    }

    return sum / n;
}

RayTracer::RayTracer(json j) {
    cout<<"RayTracer constructor"<<endl;

    if (j.contains("geometry") && j["geometry"].is_array()) {
        json geometries = j["geometry"];
        for (auto &g : geometries) {

            Material material = Material(
                    Vector3f(g["ac"][0], g["ac"][1], g["ac"][2]),
                    Vector3f(g["dc"][0], g["dc"][1], g["dc"][2]),
                    Vector3f(g["sc"][0], g["sc"][1], g["sc"][2]),
                    g["ka"], g["kd"], g["ks"], g["pc"]
            );


            Surface surface = Surface(
                    material, Vector3f(g["centre"][0], g["centre"][1], g["centre"][2])
            );

            if (g["type"] == "sphere") {
                Sphere* sphere = new Sphere(surface, g["radius"]);
                geometryList.push_back(sphere);
            } else if (g["type"] == "rectangle") {
                Rectangle* rectangle = new Rectangle(
                        surface,
                        Vector3f(g["p1"][0], g["p1"][1], g["p1"][2]),
                        Vector3f(g["p2"][0], g["p2"][1], g["p2"][2]),
                        Vector3f(g["p3"][0], g["p3"][1], g["p3"][2]),
                        Vector3f(g["p4"][0], g["p4"][1], g["p4"][2]));
                geometryList.push_back(rectangle);
            } else {
                cerr<<"unknown geometry type"<<endl;
            }

        }
    }

    if (j.contains("light") && j["light"].is_array()) {
        json lights = j["light"];
        for (auto &l : lights) {
            if(l["type"] == "point") {
                PointLight* pointLight = new PointLight(
                        Vector3f(l["id"][0], l["id"][1], l["id"][2]),
                        Vector3f(l["is"][0], l["is"][1], l["is"][2]),
                        Vector3f(l["centre"][0], l["centre"][1], l["centre"][2])
                );
                lightList.push_back(pointLight);
            } else if (l["type"] == "area") {
                AreaLight* areaLight = new AreaLight(
                        Vector3f(l["id"][0], l["id"][1], l["id"][2]),
                        Vector3f(l["is"][0], l["is"][1], l["is"][2]),
                        Vector3f(l["p1"][0], l["p1"][1], l["p1"][2]),
                        Vector3f(l["p2"][0], l["p2"][1], l["p2"][2]),
                        Vector3f(l["p3"][0], l["p3"][1], l["p3"][2]),
                        Vector3f(l["p4"][0], l["p4"][1], l["p4"][2]),
                        0
                );

                if (l.contains("n")) {
                    areaLight->setN(l["n"]);
                }

                if (l.contains("usecenter")) {
                    areaLight->setUseCenter(l["usecenter"]);
                }

                lightList.push_back(areaLight);
            } else {
                cerr<<"unknown light type"<<endl;
            }
        }
    }

    if (j.contains("output") && j["output"].is_array()) {

        json outputs = j["output"];

        auto out = outputs.begin();

        Camera camera = Camera(
                Vector3f((*out)["lookat"][0], (*out)["lookat"][1], (*out)["lookat"][2]),
                Vector3f((*out)["up"][0], (*out)["up"][1], (*out)["up"][2]),
                (*out)["fov"],
                Vector3f((*out)["centre"][0], (*out)["centre"][1], (*out)["centre"][2])
        );

        output = Output(
                (*out)["filename"],
                Vector2i((*out)["size"][0], (*out)["size"][1]),
                camera,
                Vector3f((*out)["ai"][0], (*out)["ai"][1], (*out)["ai"][2]),
                Vector3f((*out)["bkc"][0], (*out)["bkc"][1], (*out)["bkc"][2]));

        if ((*out).contains("globalillum")) {
            output.setGlobalillum((*out)["globalillum"]);
        }

        if ((*out).contains("maxbounces")) {
            output.setMaxbounces((*out)["maxbounces"]);
        }

        if ((*out).contains("probterminate")) {
            output.setProbterminate((*out)["probterminate"]);
        }

        if ((*out).contains("raysperpixel")) {
            vector<int> rays;
            for (int i=0; i<(*out)["raysperpixel"].size(); i++) {
                rays.push_back((*out)["raysperpixel"][i]);
            }

            output.setRaysPerPixel(rays);
        }

    }


}

Vector3f RayTracer::getDiffuseReflection(HitRecord& hit, Ray& ray, Vector3f vec) {
    Vector3f diffuse(0, 0, 0);

    for (const auto& light : lightList) {
        // Calculate light direction vector
        Vector3f l(0, 0, 0);

        if (light->type == "area") {
            AreaLight* areaLight = static_cast<AreaLight*>(light);

            float u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

            Vector3f sampledPosition = areaLight->p1 * (1 - u) * (1 - v) + areaLight->p2 * u * (1 - v) + areaLight->p3 * u * v + areaLight->p4 * (1 - u) * v;

            l =  sampledPosition - hit.p;
        } else {
            PointLight* pointLight = static_cast<PointLight*>(light);
            l = pointLight->center - hit.p;
        }
        float distanceToLight = l.norm();
        l.normalize();

        // Get the surface normal from the hit record
        Vector3f n = hit.n;

        // Calculate diffuse component
        float nDotL = std::max(0.0f, n.dot(l));
        Vector3f lightDiffuse = nDotL * hit.material->kd * light->is;

        // Add the result to the accumulated diffuse color
        diffuse += lightDiffuse;
    }

    return diffuse;
}

Vector3f RayTracer::getSpecularReflectance(HitRecord& hrec, Ray& ray, Vector3f& n) {
    Vector3f result(0, 0, 0);
    Material* material = hrec.material;

    for (const auto& light : lightList) {
        Vector3f lightDirection;

        if (light->type == "point") {
            PointLight* pointLight = static_cast<PointLight*>(light);
            lightDirection = (pointLight->center - hrec.p).normalized();
        }
        else if (light->type == "area") {
            AreaLight* areaLight = static_cast<AreaLight*>(light);

            float u = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

            Vector3f sampledPosition = areaLight->p1 * (1 - u) * (1 - v) + areaLight->p2 * u * (1 - v) + areaLight->p3 * u * v + areaLight->p4 * (1 - u) * v;
            lightDirection = (sampledPosition - hrec.p).normalized();
        }

        // Compute the reflection vector
        Vector3f r = (2 * n * n.dot(lightDirection) - lightDirection).normalized();

        // Compute the view direction vector
        Vector3f v = (ray.getOrigin() - hrec.p).normalized();

        // Compute the specular contribution
        float specFactor = std::pow(std::max(0.0f, r.dot(v)), material->pc);
        Vector3f specular = material->sc.cwiseProduct( light->is ) * specFactor;

        // Add the specular contribution to the result
        result += specular;
    }

    return result;
}

Vector3f RayTracer::getRandomVector(Ray& ray, Vector3f hitPoint, Vector3f n) {
    // Generate random point on unit sphere
    float x, y, z;
    do {
        x = 2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f;
        y = 2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f;
        z = 2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f;
    } while (x * x + y * y + z * z > 1.0f);
    Vector3f randomVector(x, y, z);
    randomVector.normalize();

    // Check if random vector is in the same hemisphere as the normal
    if (randomVector.dot(n) < 0.0f) {
        randomVector = -randomVector;
    }

    // Return the random direction vector
    return randomVector;
}

HitRecord RayTracer::intersect(Ray &ray) {
    HitRecord closestHitRecord;
    closestHitRecord.hit = false;
    closestHitRecord.t = std::numeric_limits<float>::max();

    for (const auto& geo : geometryList) {
        HitRecord hitRecord = geo->intersect(ray);
        if (hitRecord.hit && hitRecord.t < closestHitRecord.t) {
            closestHitRecord = hitRecord;
        }
    }

    return closestHitRecord;
}

Vector3f RayTracer::getDirectIllumination(const HitRecord& hitRecord, const Vector3f& hitPoint, const Ray& ray) {
    Vector3f directIllumination(0, 0, 0);

    for (const auto& light : lightList) {
        Vector3f lightDirection = light->getDirection(hitPoint);
        float lightDistance = light->getDistance(hitPoint);
        Ray shadowRay(hitPoint, lightDirection);
        HitRecord shadowHit = intersect(shadowRay);

        if (!shadowHit.hit || shadowHit.t > lightDistance) {
            Vector3f brdf = hitRecord.material->evalBRDF(hitRecord, -ray.direction, lightDirection);
            Vector3f lightIntensity = light->getIntensity(hitPoint);
            float nDotL = std::max(0.0f, hitRecord.n.dot(lightDirection));
            directIllumination += brdf.cwiseProduct( lightIntensity ) * nDotL;
        }
    }

    return directIllumination;
}

Vector3f RayTracer::getRandomDirectionInHemisphere(const Vector3f& normal) {
    Vector3f randomDirection;

    do {
        randomDirection = Vector3f::Random().normalized();
    } while (randomDirection.dot(normal) < 0);

    return randomDirection;
}


Vector3f RayTracer::radiance(Ray& ray, int depth) {
    HitRecord hitRecord = intersect(ray);

    float maxbounces = output.getMaxbounces() > 0 ? output.getMaxbounces() : MAX_BOUNCES;
    if (hitRecord.hit) {
        Material* material = hitRecord.material;
        Vector3f hitPoint = ray.evaluate(hitRecord.t);
        Vector3f normal = hitRecord.n;

        // Direct illumination (from light sources)
        Vector3f directIllumination = getDirectIllumination(hitRecord, hitPoint, ray);

        // Russian roulette for path termination
        if (depth > maxbounces || drand48() < p_termination_threshold) {
            return directIllumination;
        }

        // Indirect illumination (path tracing)
        Vector3f newRayDirection = getRandomDirectionInHemisphere(normal);
        Ray newRay(hitPoint, newRayDirection);

        float cosTheta = std::max(0.0f, normal.dot(newRayDirection));
        Vector3f indirectIllumination = radiance(newRay, depth + 1) * cosTheta;

        // Combine direct and indirect illumination
        Vector3f finalColor = directIllumination + indirectIllumination;

        return finalColor;
    }

    // If no intersection, return background color
    return output.bkc;
}

bool RayTracer::groupRaycastHit(const Ray& ray, float t0, float t1, HitRecord& hitRecord) {
    bool hitAnything = false;
    float closestHitDistance = t1;

    for (Geometry* geometry : this->geometryList) {
        HitRecord currentHitRecord;
        if (geometry->hit(ray, t0, closestHitDistance, currentHitRecord)) {
            hitAnything = true;
            closestHitDistance = currentHitRecord.t;
            hitRecord = currentHitRecord;
        }

    }

    return hitAnything;
}



void RayTracer::run() {
    cout<<"RayTracer run"<<endl;

    int width = output.getSize()[0];
    int height = output.getSize()[1];

    float ratio = static_cast<float>(width) / static_cast<float>(height);

    for (int i=0; i< width; i++) {
        for (int j=0; j< height; j++) {
            float u = static_cast<float>(j) / static_cast<float>(width - 1);
            float v = static_cast<float>(i) / static_cast<float>(height - 1);

            Ray ray = output.getCamera().getPrimaryRay(u, v, width, height);

        }
    }


}

