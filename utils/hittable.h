#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

//<<<<<<ch10-2 add>>>>>>>
#include "raytracer.h"
class material;

class hit_record
{
public:
    point3 p;
    vec3 normal;
    double t;

    //<<<<<<ch10-2 add>>>>>>>
    shared_ptr<material> mat;

    // ch6-4 add
    bool front_face;
    void set_face_normal(const ray &r, const vec3 &outward_normal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;

    // ====ch6-8 out&add====
    // virtual bool hit(const ray &r, double ray_tmin, double ray_tmax, hit_record &rec) const = 0;
    virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;
};

#endif