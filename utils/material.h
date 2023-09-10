#ifndef MATERIAL_H
#define MATERIAL_H

#include "raytracer.h"

class hit_record;

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
};

//<<<<<<ch10-3 add>>>>>>>
class lambertian : public material
{
public:
    lambertian(const color &a) : albedo(a) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    color albedo;
};

//<<<<<<ch10-3 add>>>>>>>
class metal : public material
{
public:
    //<<<<<<ch10-6 out&add>>>>>>>
    // metal(const color &a) : albedo(a) {}
    metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered)
        const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        attenuation = albedo;

        //<<<<<<ch10-6 out&add>>>>>>>
        // scattered = ray(rec.p, reflected);
        // return true;
        scattered = ray(rec.p, reflected + fuzz * random_unit_vector());
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    //<<<<<<ch10-6 add>>>>>>>
    double fuzz;
};

#endif