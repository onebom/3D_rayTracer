#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable
{
public:
    //<<<<<<ch10-2 out&add>>>>>>>
    // sphere(point3 _center, double _radius) : center(_center), radius(_radius){};
    sphere(point3 _center, double _radius, shared_ptr<material> _material)
        : center(_center), radius(_radius), mat(_material) {}

    // ====ch6-8 out&add====
    // bool hit(const ray &r, double ray_tmin, double ray_tmax, hit_record &rec) const override
    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        // ====ch6-8 out&add====
        // if (root <= ray_tmin || ray_tmax <= root)
        if (!ray_t.surrounds(root))
        {
            root = (-half_b + sqrtd) / a;
            // ====ch6-8 out&add====
            // if (root <= ray_tmin || ray_tmax <= root)
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        rec.normal = (rec.p - center) / radius;

        // ch6-4 add
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        //<<<<<<ch10-2 out&add>>>>>>>
        rec.mat = mat;

        return true;
    }

private:
    point3 center;
    double radius;
    //<<<<<<ch10-2 out&add>>>>>>>
    shared_ptr<material> mat;
};

#endif