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

void RayTracer::outputPPM() {
    cout << "RayTracer::outputPPM()" << endl;
    // Set up the camera
    Vector3f cameraPosition = output->getCamera().getCenter();
    Vector3f cameraDirection = (output->getCamera().getLookat() - cameraPosition).normalized();
    Vector3f cameraRight = cameraDirection.cross(output->getCamera().getUp()).normalized();
    Vector3f cameraUp = output->getCamera().getUp();

    // Compute the distance to the image plane
    float imagePlaneDist = (output->getHeight() / 2.0f) / tan(output->getCamera().getFov() / 2.0f);

    // Open the output file
    std::ofstream outfile(output->getFilename());

    // Write the PPM header
    outfile << "P3\n" << output->getWidth() << " " << output->getHeight() << "\n255\n";

    // Loop over each pixel in the image and compute the color
    for (int j = output->getHeight() - 1; j >= 0; j--) {
        for (int i = 0; i < output->getWidth(); i++) {
            // Compute the ray direction for this pixel
            float u = (i + 0.5f) / output->getWidth();
            float v = (j + 0.5f) / output->getHeight();
            Vector3f rayDirection = (u - 0.5f) * output->getWidth() * cameraRight
                                    + (v - 0.5f) * output->getHeight() * cameraUp
                                    - imagePlaneDist * cameraDirection;

            // Compute the intersection of the ray with the sphere
            Ray ray(cameraPosition, rayDirection);
            float t;
            Sphere* sphere = (Sphere*) geometryList[0];
            if (geometryList[0]->intersect(ray, t)) {
                // Compute the point of intersection and the surface normal
                Vector3f pointOfIntersection = ray.at(t);
                Vector3f surfaceNormal = (pointOfIntersection - sphere->getCenter()).normalized();

                // Compute the ambient, diffuse, and specular terms
                Vector3f ambientTerm = output->getAmbientLight() * sphere->getKa() * sphere->getPc();
                float diffuseTerm = sphere->getKd() * std::max(0.0f, surfaceNormal.dot(-rayDirection)) * sphere->getPc();
                Vector3f reflectionDirection = 2.0f * surfaceNormal * surfaceNormal.dot(-rayDirection) + rayDirection;
                Vector3f specularTerm = sphere->getKs() * std::pow(std::max(0.0f, reflectionDirection.dot(-cameraDirection)), sphere.shininess()) * sphere.pc();

                // Compute the final color and write it to the file
                Vector3f color = ambientTerm + diffuseTerm + specularTerm;
                outfile << (int)std::round(std::min(255.0f, color.x())) << " "
                        << (int)std::round(std::min(255.0f, color.y())) << " "
                        << (int)std::round(std::min(255.0f, color.z())) << " ";            } else {
                outfile << (int) (255.0f * output->getBackgroundColor()[0]) << " " << (int) (255.0f * output->getBackgroundColor()[1]) << " " << (int) (255.0f * output->getBackgroundColor()[2]) << " ";
            }
}

//void writePPM(const Output& output, const Sphere& sphere) {
//    // Set up the camera
//    Vector3f cameraPosition = output.camera().center();
//    Vector3f cameraDirection = (output.camera().lookat() - cameraPosition).normalized();
//    Vector3f cameraRight = cameraDirection.cross(output.camera().up()).normalized();
//    Vector3f cameraUp = cameraRight.cross(cameraDirection);
//
//    // Compute the distance to the image plane
//    float imagePlaneDist = (output.height() / 2.0f) / tan(output.camera().fov() / 2.0f);
//
//    // Open the output file
//    std::ofstream outfile(output.filename());
//
//    // Write the PPM header
//    outfile << "P3\n" << output.width() << " " << output.height() << "\n255\n";
//
//    // Loop over each pixel in the image and compute the color
//    for (int j = output.height() - 1; j >= 0; j--) {
//        for (int i = 0; i < output.width(); i++) {
//            // Compute the ray direction for this pixel
//            float u = (i + 0.5f) / output.width();
//            float v = (j + 0.5f) / output.height();
//            Vector3f rayDirection = (u - 0.5f) * output.width() * cameraRight
//                                           + (v - 0.5f) * output.height() * cameraUp
//                                           - imagePlaneDist * cameraDirection;
//
//            // Compute the intersection of the ray with the sphere
//            Ray ray(cameraPosition, rayDirection);
//            float t;
//            if (sphere.intersect(ray, t)) {
//                // Compute the point of intersection and the surface normal
//                Vector3f pointOfIntersection = ray.pointAt(t);
//                Vector3f surfaceNormal = (pointOfIntersection - sphere.center()).normalized();
//
//                // Compute the ambient, diffuse, and specular terms
//                Vector3f ambientTerm = output.ambientLight() * sphere.ka() * sphere.pc();
//                Vector3f diffuseTerm = sphere.kd() * std::max(0.0f, surfaceNormal.dot(-rayDirection)) * sphere.pc();
//                Vector3f reflectionDirection = 2.0f * surfaceNormal * surfaceNormal.dot(-rayDirection) + rayDirection;
//                Vector3f specularTerm = sphere.ks() * std::pow(std::max(0.0f, reflectionDirection.dot(-cameraDirection)), sphere.shininess()) * sphere.pc();
//
//                // Compute the final color and write it to the file
//                Vector3f color = ambientTerm + diffuseTerm + specularTerm;
//                outfile << (int)std::round(std::min(255.0f, color.x())) << " "
//                        << (int)std::round(std::min(255.0f, color.y())) << " "
//                        << (int)std::round(std::min(255.0f, color.z())) << " ";
//            }
//            else {
//                // No intersection, so write the background color to the file
//                outfile << (int)std::round(std::min(255.0f, output.backgroundColor().x())) << " "
//                        << (int)std::round(std::min(255.0f, output.backgroundColor().y())) << " "
//                        << (int)std::round(std::min(255.0f, output.backgroundColor().z())) << " ";
//            }
//        }
//        outfile << std::endl;
//    }