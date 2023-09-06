#include "raytracer.h"

#include "color.h"
// #include "ray.h"
// #include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

color ray_color(const ray &r, int depth, const hittable &world)
{
    hit_record rec;

    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec))
    {
        // // point3 target = rec.p + rec.normal + random_unit_vector();
        // point3 target = rec.p + random_in_hemisphere(rec.normal);
        // // return 0.5 * (rec.normal + color(1, 1, 1));
        // return 0.5 * ray_color(ray(rec.p, target - rec.p), depth - 1, world);
        ray scatterd;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scatterd))
            return attenuation * ray_color(scatterd, depth - 1, world);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene()
{
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -2; a < 2; a++)
    {
        for (int b = -2; b < 2; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main()
{

    // Image
    // auto aspect_ratio = 16.0 / 9.0;
    // int image_width = 400;
    // // Calculate the image height, and ensure that it's at least 1.
    // int image_height = static_cast<int>(image_width / aspect_ratio);
    // // image_height = (image_height < 1) ? 1 : image_height;
    // const int samples_per_pixel = 100;
    // const int max_depth = 10;
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // World
    auto world = random_scene();
    // auto R = cos(pi / 4);
    // hittable_list world;

    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    // // auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    // auto material_left = make_shared<dielectric>(1.5);
    // auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    // world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    // world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    // world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
    // world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_center));
    // world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));

    // Camera
    // auto focal_length = 1.0;
    // auto viewport_height = 2.0;
    // auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
    // auto camera_center = point3(0, 0, 0);
    // camera cam;
    // camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20, aspect_ratio);
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
    // // Calculate the vectors across the horizontal and down the vertical viewport edges.
    // auto viewport_u = vec3(viewport_width, 0, 0);
    // auto viewport_v = vec3(0, -viewport_height, 0);

    // // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    // auto pixel_delta_u = viewport_u / image_width;
    // auto pixel_delta_v = viewport_v / image_height;

    // // Calculate the location of the upper left pixel.
    // // viewport_upper_left = Q = (0,0,0)-(0,0,1.0)-(v_w,0,0)/2-(0,-2.0,0)/2
    // auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    // auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n"
              << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j)
    {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            // // 각 pixel에 대해서 ray를 쏘고
            // auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            // auto ray_direction = pixel_center - camera_center;
            // ray r(camera_center, ray_direction);

            // color pixel_color = ray_color(r, world);

            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, max_depth, world);
            }
            // write_color(std::cout, pixel_color);
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::clog << "\rDone.                 \n";
}