//
// Created by xu zhang on 2023-01-07.
//

#include "Geometry.h"

Vector3f Material::evalBRDF(const HitRecord& hitRecord, const Vector3f& inDirection, const Vector3f& outDirection) const {
    // Simple Lambertian BRDF
    float lambertian = max(0.0f, hitRecord.n.dot(inDirection));
    return hitRecord.color * (kd * lambertian);
}

HitRecord Sphere::intersect(const Ray& ray) const {
    HitRecord hitRecord;

    Vector3f oc = ray.getOrigin() - center;
    float a = ray.getDirection().dot(ray.getDirection());
    float b = 2.0f * oc.dot(ray.getDirection());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        if (t > 0) {
            hitRecord.hit = true;
            hitRecord.t = t;
            hitRecord.p = ray.evaluate(t);
            hitRecord.n = (hitRecord.p - center).normalized();
            hitRecord.material = &getSurface().material;
            return hitRecord;
        }

        t = (-b + sqrt(discriminant)) / (2.0f * a);
        if (t > 0) {
            hitRecord.hit = true;
            hitRecord.t = t;
            hitRecord.p = ray.evaluate(t);
            hitRecord.n = (hitRecord.p - center).normalized();
            hitRecord.material = &getSurface().material;
            return hitRecord;
        }
    }

    hitRecord.hit = false;
    return hitRecord;
}

HitRecord Rectangle::intersect(const Ray& ray) const{
    const float EPSILON = 1e-6f;
    HitRecord hit;
    hit.hit = false;
    hit.material = &getSurface().material;

    Vector3f edge1 = p2 - p1;
    Vector3f edge2 = p3 - p1;
    Vector3f normal = edge1.cross(edge2).normalized();

    float ndotdR = normal.dot(ray.direction);
    if (fabs(ndotdR) < EPSILON) {
        // Ray and rectangle are parallel
        return hit;
    }

    float t = (normal.dot(p1) - normal.dot(ray.origin)) / ndotdR;
    if (t < 0) {
        // Intersection is behind the ray origin
        return hit;
    }

    Vector3f intersection = ray.origin + t * ray.direction;

    Vector3f C;

    // Test edge p1-p2
    Vector3f vp1 = intersection - p1;
    C = edge1.cross(vp1);
    if (normal.dot(C) < 0) {
        return hit;
    }

    // Test edge p2-p3
    Vector3f edge3 = p3 - p2;
    Vector3f vp2 = intersection - p2;
    C = edge3.cross(vp2);
    if (normal.dot(C) < 0) {
        return hit;
    }

    // Test edge p3-p4
    Vector3f edge4 = p4 - p3;
    Vector3f vp3 = intersection - p3;
    C = edge4.cross(vp3);
    if (normal.dot(C) < 0) {
        return hit;
    }

    // Test edge p4-p1
    Vector3f edge5 = p1 - p4;
    Vector3f vp4 = intersection - p4;
    C = edge5.cross(vp4);
    if (normal.dot(C) < 0) {
        return hit;
    }

    hit.hit = true;
    hit.t = t;
    hit.p = intersection;
    hit.n = normal;

    return hit;
}

bool Sphere::hit(const Ray &ray, float t0, float closestHitDistance, HitRecord &currentHitRecord) {
    Vector3f oc = ray.getOrigin() - center;
    float a = ray.getDirection().dot(ray.getDirection());
    float b = 2.0f * oc.dot(ray.getDirection());
    float c = oc.dot(oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        if (t > t0 && t < closestHitDistance) {
            currentHitRecord.hit = true;
            currentHitRecord.t = t;
            currentHitRecord.p = ray.evaluate(t);
            currentHitRecord.n = (currentHitRecord.p - center).normalized();
            currentHitRecord.material = &getSurface().material;
            return true;
        }

        t = (-b + sqrt(discriminant)) / (2.0f * a);
        if (t > t0 && t < closestHitDistance) {
            currentHitRecord.hit = true;
            currentHitRecord.t = t;
            currentHitRecord.p = ray.evaluate(t);
            currentHitRecord.n = (currentHitRecord.p - center).normalized();
            currentHitRecord.material = &getSurface().material;
            return true;
        }
    }

    return false;
}

bool Rectangle::hit(const Ray &ray, float t0, float closestHitDistance, HitRecord &currentHitRecord) {
    const float EPSILON = 1e-6f;
    HitRecord hit;
    hit.hit = false;
    hit.material = &getSurface().material;

    Vector3f edge1 = p2 - p1;
    Vector3f edge2 = p3 - p1;
    Vector3f normal = edge1.cross(edge2).normalized();

    float ndotdR = normal.dot(ray.direction);
    if (fabs(ndotdR) < EPSILON) {
        // Ray and rectangle are parallel
        return false;
    }

    float t = (normal.dot(p1) - normal.dot(ray.origin)) / ndotdR;
    if (t < t0 || t > closestHitDistance) {
        // Intersection is behind the ray origin
        return false;
    }

    Vector3f intersection = ray.origin + t * ray.direction;

    Vector3f C;

    // Test edge p1-p2
    Vector3f vp1 = intersection - p1;
    C = edge1.cross(vp1);
    if (normal.dot(C) < 0) {
        return false;
    }

    // Test edge p2-p3
    Vector3f edge3 = p3 - p2;
    Vector3f vp2 = intersection - p2;
    C = edge3.cross(vp2);
    if (normal.dot(C) < 0) {
        return false;
    }

    // Test edge p3-p4
    Vector3f edge4 = p4 - p3;
    Vector3f vp3 = intersection - p3;
    C = edge4.cross(vp3);
    if (normal.dot(C) < 0) {
        return false;
    }

    // Test edge p4-p1
    Vector3f edge5 = p1 - p4;
    Vector3f vp4 = intersection - p4;
    C = edge5.cross(vp4);
    if (normal.dot(C) < 0) {
        return false;
    }

    currentHitRecord.hit = true;
    currentHitRecord.t = t;
    currentHitRecord.p = intersection;
    currentHitRecord.n = normal;
    currentHitRecord.material = &getSurface().material;

    return true;
}

