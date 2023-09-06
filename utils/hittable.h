#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "raytracer.h"

class material;

class hit_record
{
public:
    point3 p; // intersection point
    vec3 normal;
    shared_ptr<material> mat_ptr;
    double t; // intersection solution

    bool front_face; //?

    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        // NOTE: outward_normal은 항상 unit length를 가져야한다.
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    virtual bool hit(const ray &r, double ray_tmin, double ray_tmax, hit_record &rec) const = 0;
};

#endif