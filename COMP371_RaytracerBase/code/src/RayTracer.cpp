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
const int MAX_BOUNCES = 2; // number of bounces

int current_path_sample = 0; // current path sample
int current_path_bounce = 0; // current path bounce
bool stratified_sampling = false; // stratified sampling or not

const unsigned int cells_x = 10; // number of cells in x direction
const unsigned int cells_y = 10; // number of cells in y direction
const unsigned int cells_size = NB_SAMPLES_PER_PIXEL / (cells_x * cells_y); // number of samples per cell

float p_termination_threshold = 0.1f; // probability of termination threshold
float p = 0.5f; // probability of termination
float p_reflection = 0.1f; // probability of reflection

float cos_theta; // cosine of theta
const Vector3f light_color(0, 0, 0); // light color

float jitterAmount = 0.5f; // jitter amount
// Li and Le
Vector3f Li(0, 0, 0);
Vector3f Le(0, 0, 0);

Vector3f radiance_accumulator(0,0,0);


double getMonteCarloEstimate(int n) {
    double x, sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += exp(sin(3 * pow(drand48(), 2)));
    }

    return sum / n;
}

Vector3f clampVectorXf(Eigen::Vector3f value, float min, float max) {
    Eigen::Vector3f clamped;
    if(value(0) < min) {
        clamped(0) = min;
    } else if(value(0) > max) {
        clamped(0) = max;
    } else {
        clamped(0) = value(0);
    }

    if(value(1) < min) {
        clamped(1) = min;
    }
    if(value(1) > max) {
        clamped(1) = max;
    } else {
        clamped(1) = value(1);
    }

    if(value(2) < min) {
        clamped(2) = min;
    }
    if(value(2) > max) {
        clamped(2) = max;
    } else {
        clamped(2) = value(2);
    }
    return clamped;
};


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


            Surface surface = Surface(material);

            if (g["type"] == "sphere") {
                Vector3f centre = Vector3f(g["centre"][0], g["centre"][1], g["centre"][2]);
                Sphere* sphere = new Sphere(surface, g["radius"], centre);
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

RayTracer::~RayTracer() {
    for (auto &g : geometryList) {
        delete g;
    }

    for (auto &l : lightList) {
        delete l;
    }

}

Vector3f RayTracer::getAmbientReflectance(Vector3f ac, float ka) {
    Vector3f ambientReflection = ka * ac.cwiseProduct(output.ai);
    return ambientReflection;
};

Vector3f RayTracer::getDiffuseReflection(HitRecord& hrec, Ray& ray) {
    float kd, ks, pc;
    Vector3f dc, sc;
    if (hrec.material != nullptr && hrec.material != NULL) {
        kd = hrec.material->kd; // Diffuse coefficient
        ks = hrec.material->ks; // Specular coefficient
        dc = hrec.material->dc; // Diffuse color
        sc = hrec.material->sc; // Specular color
        pc = hrec.material->pc; // Phong coefficient
    } else {
        kd = 1.0f;
    }

    Vector3f n = hrec.n;


    Vector3f viewing_ray = -1*(ray.getDirection());
    viewing_ray = viewing_ray.normalized();
    float cos_theta, distance, attenuation_factor;
    Vector3f light_ray(0,0,0), diffuseColor(0,0,0);
    Vector3f lightColor(1,1,1);
    Vector3f id(1,1,1);

    for(Light* l : lightList) {
        if (l->type == "point") {
            PointLight* pl = (PointLight*) l;
            light_ray = (pl->center - hrec.p);
            light_ray = light_ray.normalized();
            cos_theta = max(n.dot(light_ray), 0.0f);
            distance = light_ray.norm() * light_ray.norm();
            attenuation_factor = 1 / distance;
            if(cos_theta > 0.0f) {
                diffuseColor += (kd * dc * cos_theta).cwiseProduct(id);
            }
        }

    }
    return diffuseColor;
};


Vector3f RayTracer::getSpecularReflectance(HitRecord& hrec, Ray& ray) {
    float ks, pc; Vector3f sc, is(1,1,1);
    if (hrec.material != nullptr && hrec.material != NULL) {
        ks = hrec.material->ks; // Specular coefficient
        sc = hrec.material->sc; // Specular color
        pc = hrec.material->pc; // Phong coefficient
    } else {
        Vector3f _sc(1,1,1);
        ks = 1.0f;
        sc = _sc;
        pc = 10;
    }
    Vector3f x = ray.evaluate(hrec.t);
    Vector3f light_ray, half;
    Vector3f viewing_ray = -1*(ray.getDirection());
    viewing_ray = viewing_ray.normalized();
    float cos_theta, cos_alpha, shininess = 0.0f;
    Vector3f n = hrec.n;
    for(Light* l : lightList) {
        if (l->type == "point") {
            PointLight* pl = (PointLight*) l;
            light_ray = (pl->center - x);
            light_ray = light_ray.normalized();
            cos_theta = max(n.dot(light_ray), 0.0f);
            half = viewing_ray + light_ray;
            half = half.normalized();
            // cos_alpha = max(0.0, n.dot(half));

            Vector3f reflection_ray = (2*n)*(cos_theta) - light_ray;
            cos_alpha = reflection_ray.dot(viewing_ray);

            if(cos_theta > 0.0) {
                shininess += pow(cos_alpha, pc);
            }
        }

    }

    Vector3f lightColor(1,1,1);
    Vector3f specularReflectance = (ks * sc * shininess).cwiseProduct(is);

    return specularReflectance;
};


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

Vector3f RayTracer::radiance(HitRecord& currentHit, Vector3f o, Vector3f d) {

    Vector3f hitPoint;

    for(Geometry* go : geometryList) {

        Ray ray(o, d);
        bool hitSomething = go->hit(ray, 0, 1000, currentHit);

        if(hitSomething) {
            hitPoint = ray.evaluate(currentHit.t);
            Vector3f random_dir_vec(0,0,0);


            getRandomVector(ray, hitPoint.normalized(), currentHit.n);
            random_dir_vec = random_dir_vec.normalized(); // Normalize random vector

            Vector3f n = currentHit.n; // Normal at hit point

            cos_theta = max(n.dot(d), 0.0f); // Cosine of angle between normal and ray direction
            p = drand48();
            Le = getAmbientReflectance(currentHit.material->ac, currentHit.material->ka);

            if(current_path_bounce > MAX_BOUNCES || p < p_termination_threshold) {


                Li = Vector3f (0,0,0);

                for(Light* l : lightList) {
                    Li += l->computeContribution(ray, currentHit);
                }

                // Reset global path bounces
                // -------------------------
                current_path_bounce = 0;
                return Li;
            } else {
                return (currentHit.material->dc.cwiseProduct(radiance(currentHit, hitPoint, random_dir_vec) * cos_theta));
            }

            ++current_path_bounce;

        } else {
            // No hit => background color
            // --------------------------
            return output.bkc;
        }
    }
};



Vector3f RayTracer::getRandomDirectionInHemisphere(const Vector3f& normal) {
    Vector3f randomDirection;

    do {
        randomDirection = Vector3f::Random().normalized();
    } while (randomDirection.dot(normal) < 0);

    return randomDirection;
}


Vector3f RayTracer::computeGlobalIlluminationContribution(const Ray& ray, const HitRecord& hitRecord) {
    Vector3f globalIlluminationContribution(0, 0, 0);
    int numSamples=100;
    int a = 0, b = 0;
    vector<int>& raysperpixel = output.raysPerPixel;

//    if (raysperpixel.empty()) {
//        // Default rays per pixel value if not provided
//        numSamples = 10;
//    } else if (raysperpixel.size() == 1) {
//        numSamples = raysperpixel[0];
//    } else if (raysperpixel.size() == 2) {
//        a = raysperpixel[0];
//        b = raysperpixel[1];
//        numSamples = a * a * b;
//    } else if (raysperpixel.size() == 3) {
//        a = raysperpixel[0];
//        b = raysperpixel[1];
//        int samplesPerCell = raysperpixel[2];
//        numSamples = a * b * samplesPerCell;
//    } else {
//        // Default rays per pixel value if not provided
//        numSamples = 10;
//    }
    // Loop through the samples
    for (int i = 0; i < numSamples; ++i) {
        Vector3f randomDirection = getRandomDirectionInHemisphere(hitRecord.n);

        Ray newRay(hitRecord.p, randomDirection);

//        globalIlluminationContribution += radiance(newRay, 1);
    }

    globalIlluminationContribution /= numSamples;

    return globalIlluminationContribution;
}


bool RayTracer::groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn) {

    // HIT RECORD SETUP
    // ----------------
    HitRecord* currentHit = new HitRecord(t1); // Start ray at max range
    HitRecord* closestHit = currentHit;
    HitRecord* shadowHit(currentHit);

    // Iteration parameters
    // --------------------
    float minT1 = t1; // The minimum closest hit to viewer
    bool hitSomethingAtAll = false; // Should return bkg color if no hit

    // AMBIENT LIGHT
    // -------------
    float ka;
    Vector3f ai, ac, dc;
    ai = output.ai;

    for(Geometry* go : geometryList) {

        // GEOMETRY INTERSECTION TEST
        // --------------------------
        bool hitSomething = go->hit(ray, t0, t1, *currentHit);

        if(hitSomething) {
            // NO BKG COLOR TEST
            // -----------------
            hitSomethingAtAll = true;

            // CLOSEST HIT: MIN T TEST
            // -----------------------
            if(abs(currentHit->t) < abs(minT1)) {
                minT1 = currentHit->t;
                t1 = minT1;
                closestHit = currentHit;

                // LIGHT PARAMETERS
                // ----------------
                Vector3f light_sum(0,0,0); // THE LIGHT SUM
                Vector3f lightOutput; // THE FINAL OUTPUT

                // GLOBAL ILLUMINATION STUFF
                // -------------------------------
                ka = closestHit->material->ka; // Ambient coefficient
                ac = closestHit->material->ac; // Ambient color
                dc = closestHit->material->dc; // Diffuse material color used for integration
                Vector3f aL = ka * ac.cwiseProduct(ai);

                // LOCAL ILLUMINATION : PRIMARY RAY
                // --------------------------------
                if(!output.getGlobalillum()) {
                    for(Light* l : lightList) {
                        light_sum += l->computeContribution(ray, *closestHit);
                    }
                    light_sum += aL; // THE REFLECTION EQUATION
                    // CLAMP
                    // -----
                    lightOutput = clampVectorXf(light_sum, 0.0, 1.0);

                    closestHit->color = lightOutput;
                } else {
                    srand((unsigned)time( NULL ));

                    // Iteration parameters
                    // --------------------
                    Vector3f light_path(0,0,0);
                    Vector3f indirect_iillum(0,0,0);
                    Vector3f direct_illum(0,0,0);
                    Vector3f random_dir_vec(0,0,0);
                    Vector3f hitPoint = ray.evaluate(closestHit->t);
                    getRandomVector(ray, hitPoint.normalized(), closestHit->n);
                    random_dir_vec = random_dir_vec.normalized();
                    Vector3f n = currentHit->n;
                    cos_theta = max(n.dot(random_dir_vec), 0.0f); // d = ki

                    Le = getDiffuseReflection(*closestHit, ray); // Le = kd * Ld
                    // -----------------------------
                    for(Light* l : lightList) {
                        direct_illum += l->computeContribution(ray, *closestHit);
                    }

                    if (stratified_sampling) {
                        for(int i = 0, j = 0; i < cells_y; i++) {
                            do {
                                indirect_iillum += radiance(*closestHit, hitPoint, random_dir_vec) * cos_theta;
                                j++;
                            } while (j < cells_x);
                            indirect_iillum /= cells_x;

                            light_path += indirect_iillum;
                            light_path *= 1.0f / M_PI;

                            indirect_iillum = Vector3f(0,0,0);
                            j = 0;
                        }
                    } else {
                        do {
                            indirect_iillum += (closestHit->material->dc.cwiseProduct(radiance(*closestHit, hitPoint, random_dir_vec) * cos_theta));

                            ++current_path_sample;

                        } while (current_path_sample < NB_SAMPLES_PER_PIXEL);
                        light_path += indirect_iillum;
                        light_path /= (NB_SAMPLES_PER_PIXEL);
                    }

                    light_path += direct_illum;

                    light_path(0) = pow(light_path(0), 1.0f/2.2f);
                    light_path(1) = pow(light_path(1), 1.0f/2.2f);
                    light_path(2) = pow(light_path(2), 1.0f/2.2f);

                    closestHit->color = light_path;
                    current_path_sample = 0;
                }
            } // End Local vs. Global

            if (antialiasing) {
                handleAntialiasing(ray, *closestHit);
            }
        } // Closest hit to viewer
    }// Surface list loop
    if(!hitSomethingAtAll) {
        hitReturn.color = output.bkc;
        return false;
    } else {
        hitReturn.color = closestHit->color;
    }
    return true;
};

Vector3f RayTracer::computeShading(Ray& ray, HitRecord& hit) {
    // Calculate the ambient component of the lighting
    Vector3f ambient = hit.material->ka * output.ai;

    // Calculate the diffuse and specular components of the lighting
    Vector3f diffuse = getDiffuseReflection(hit, ray);
    Vector3f specular = getSpecularReflectance(hit, ray);

    // Combine ambient, diffuse, and specular components to get the final color
    Vector3f finalColor = ambient + diffuse + specular;

    // Apply any other shading effects (e.g., reflections, refractions, etc.) and
    // add them to the final color if necessary

    return finalColor;
}


void computeOrthogonalBasis(const Vector3f& direction, Vector3f& u, Vector3f& v) {
    Vector3f temp;
    if (std::abs(direction(0)) > std::abs(direction(1))) {
        temp = Vector3f(direction(2), 0, -direction(0)) / sqrt(direction(0) * direction(0) + direction(2) * direction(2));
    } else {
        temp = Vector3f(0, -direction(2), direction(1)) / sqrt(direction(1) * direction(1) + direction(2) * direction(2));
    }
    u = direction.cross(temp);
    v = direction.cross(u);
}

Ray RayTracer::generateJitteredRay(const Ray& ray) {
    // Generate random offsets in the range [-jitterAmount, jitterAmount]
    float xOffset = jitterAmount * (drand48() * 2 - 1);
    float yOffset = jitterAmount * (drand48() * 2 - 1);

    // Apply the offsets to the ray's starting position
    Vector3f jitteredOrigin = ray.origin + Vector3f(xOffset, yOffset, 0);

    // Create a new ray with the jittered starting position
    Ray jitteredRay(jitteredOrigin, ray.direction);

    return jitteredRay;
}

Ray RayTracer::generateStratifiedJitteredRay(const Ray& originalRay, int a, int b, int sampleIndex) {
    // Compute the orthogonal basis for the given direction
    Vector3f u, v;
    computeOrthogonalBasis(originalRay.getDirection(), u, v);

    int row = sampleIndex / (a * b);
    int col = sampleIndex % (a * b);

    float uOffset = (col + (float)rand() / RAND_MAX) / (a * b);
    float vOffset = (row + (float)rand() / RAND_MAX) / a;

    Vector3f jitteredDirection = originalRay.getDirection() + (u * uOffset) + (v * vOffset);
    jitteredDirection.normalize();

    return Ray(originalRay.getOrigin(), jitteredDirection);
}


void RayTracer::handleAntialiasing(const Ray& ray, HitRecord& hitRecord) {
    int raysPerPixel;
    int a = 0, b = 0;
    bool stratifiedSampling = false;
    vector<int>& raysperpixel = output.raysPerPixel;

    if (raysperpixel.empty()) {
        // Default rays per pixel value if not provided
        raysPerPixel = 10;
    } else if (raysperpixel.size() == 1) {
        raysPerPixel = raysperpixel[0];
    } else if (raysperpixel.size() == 2) {
        stratifiedSampling = true;
        a = raysperpixel[0];
        b = raysperpixel[1];
        raysPerPixel = a * a * b;
    } else if (raysperpixel.size() == 3) {
        stratifiedSampling = true;
        a = raysperpixel[0];
        b = raysperpixel[1];
        int samplesPerCell = raysperpixel[2];
        raysPerPixel = a * b * samplesPerCell;
    } else {
        // Default rays per pixel value if not provided
        raysPerPixel = 10;
    }

    // Initialize color accumulator
    Vector3f accumulatedColor(0, 0, 0);

    // Generate jittered rays and accumulate their color contributions
    for (int i = 0; i < raysPerPixel; ++i) {
        Ray jitteredRay;

        if (stratifiedSampling) {
            // Generate stratified jittered ray
            jitteredRay = generateStratifiedJitteredRay(ray, a, b, i);
        } else {
            // Generate uniform jittered ray
            jitteredRay = generateJitteredRay(ray);
        }

        // Call the groupRaycastHit function with the jittered ray
        HitRecord tempHit;
        if (groupRaycastHit(jitteredRay, MIN_RAY_LENGTH, MAX_RAY_LENGTH, tempHit)) {
            accumulatedColor += tempHit.color;
        } else {
            accumulatedColor += output.bkc;
        }
    }

    // Average the accumulated color
    hitRecord.color = accumulatedColor / raysPerPixel;
}

float degToRad(float deg) {
    return deg * M_PI / 180;
}

float radToDeg(float rad) {
    return rad * 180 / M_PI;
}

float RayTracer::computePixelSizeDeltaHorizontal(float horizontal_fov, float height) {
    return 2 * tan(degToRad(horizontal_fov / 2)) / height;
}

float RayTracer::computePixelSizeDeltaVertical(float vertical_fov, float height) {
    return 2 * tan(degToRad(vertical_fov / 2)) / height;
}

int RayTracer::save_ppm(const std::vector<float>& buffer, int dimx, int dimy) {
    std::ofstream ofs;
    ofs.open(output.filename, std::ios::out | std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Error: Unable to open output file \"" << output.filename << "\" for writing." << std::endl;
        return -1;
    }

    ofs << "P6\n" << dimx << " " << dimy << "\n255\n";
    for (unsigned int j = 0; j < dimy; j++) {
        for (unsigned int i = 0; i < dimx; i++) {
            ofs << (char)(255.0 * buffer[3 * j * dimx + 3 * i + 0])
                << (char)(255.0 * buffer[3 * j * dimx + 3 * i + 1])
                << (char)(255.0 * buffer[3 * j * dimx + 3 * i + 2]);
        }
    }

    std::cout << std::endl;
    ofs.close();
    return 0;
}


void RayTracer::run() {
    // Get the camera parameters
    Camera camera = output.getCamera();
    Vector3f eye = camera.getCentre();
    Vector3f lookAt = camera.getLookat();
    Vector3f up = camera.getUp();
    Vector3f left = lookAt.cross(up);
    float horizontal_fov = camera.getFov();
    float width = output.size[0];
    float height = output.size[1];
    // Calculate the image plane size
    float aspectRatio = width / height;
    float vertical_fov = radToDeg(2*atan2(tan(degToRad(horizontal_fov/2)), aspectRatio));


    float pixelSizeDeltaHorizontal = computePixelSizeDeltaHorizontal(horizontal_fov, height);
    float pixelSizeDeltaVertical = computePixelSizeDeltaVertical(vertical_fov, width);

    float scale_v = tan(degToRad(horizontal_fov) / 2);
    float scale_u = tan(degToRad(vertical_fov) / 2);

    float delta = computePixelSizeDeltaHorizontal(horizontal_fov, height); // The direct pixel size

    Vector3f referencePoint = eye + (1)*lookAt;
    Vector3f topCenter = referencePoint + (up * (delta*(height/2)));
    Vector3f topLeft = topCenter - (left * (delta*(width/2)));

    // Compute the camera basis
    Vector3f w = (eye - lookAt).normalized();
    Vector3f u = (up.cross(w)).normalized();
    Vector3f v = (w.cross(u));

    // Initialize the image buffer
    vector<float> buffer(3 * width * height);

    Ray currentRay;
    Vector3f ray_nds; // Normalized device coordinates

    HitRecord* rayColor = new HitRecord(MAX_RAY_LENGTH);

    bool isHit = false;

    for( int j = height-1; j>=0; j--){
        for (int i = 0; i< width; i++) {
            ray_nds = topLeft + (i*delta+delta/2)*left - (j*delta+delta/2)*up - eye; // Adjusts cam position

            currentRay = Ray(eye, ray_nds);

            isHit = groupRaycastHit(currentRay, MIN_RAY_LENGTH, MAX_RAY_LENGTH, *rayColor);

            if(!isHit){
                continue;
            }

            buffer[3 * j * width + 3 * i + 0] = rayColor->color[0];
            buffer[3 * j * width + 3 * i + 1] = rayColor->color[1];
            buffer[3 * j * width + 3 * i + 2] = rayColor->color[2];
        }
    }

    save_ppm(buffer, width, height);

}

