//
// Created by xu zhang on 2023-03-23.
//

#include "HitRecord.h"

Vector3f Ray::evaluate(float t) const {
    return origin + t * direction;
}