//
// Created by xu zhang on 2023-01-07.
//

#include "Geometry.h"

bool Sphere::intersect(Ray ray, float d) {
    Vector3f oc = ray.getOrigin() - center;
    double a = ray.getDirection().dot(ray.getDirection());
    double b = 2.0 * oc.dot(ray.getDirection());
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    } else {
        double t1 = (-b - sqrt(discriminant)) / (2.0 * a);
        double t2 = (-b + sqrt(discriminant)) / (2.0 * a);
        if (t1 > 0 && t1 < d) {
            d = t1;
            return true;
        } else if (t2 > 0 && t2 < d) {
            d = t2;
            return true;
        } else {
            return false;
        }
    }
}