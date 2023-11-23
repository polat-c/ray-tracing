// https://raytracing.github.io/books/RayTracingInOneWeekend.html 

#include "general.h"
#include "color.h"
// #include "ray.h" // in general included
// #include "vec3.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "cuboid.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"

#include <iostream>

using namespace std;

// At its core, a ray tracer sends rays through pixels and computes the color seen in the direction of those rays.
void main_schene() {

    // WORLD
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.2, 0.2, 0.2));
    auto material_center = make_shared<lambertian>(color(0.0, 0.0, 0.6));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    // Our world
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));
    //world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));

    // world.add(make_shared<sphere>(point3( 0.0,    3.0, -1.0),   0.5, material_center));
    world.add(make_shared<cuboid>(point3(0.0, 3.5, -1.0), 0.8, 0.8, 0.8, material_center, 
        pi/4, 2*pi/3, pi/6, "euler"));
    // ===============================================================================
    auto material_metal_cuboid  = make_shared<metal>(color(0.95, 0.95, 0.95), 0.05);
    // world.add(make_shared<cuboid>(point3(-1.0, 0.0, -1.0), 0.8, 0.8, 0.8, material_metal_cuboid));
    world.add(make_shared<cuboid>(point3(-3.0, 3.0, -1.0), 2.5, 2.5, 2.5, material_metal_cuboid, 
        0, pi/3, pi/6, "euler"));
    // ===============================================================================
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left)); // use inverted radius to invert their normals
    // --> thus making a hollow glass sphere, if you combine the above 2 surfaces
    // ===============================================================================
    world.add(make_shared<sphere>(point3( 1.0,    2.5, -1.0),   0.5, material_right));

    // make_shared<T> constructs an object of type T and wraps it in a shared_ptr using args as the parameter list for the constructor of T
    // world = hittable_list(make_shared<bvh_node>(world));

    // CAMERA
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400; // 1200
    cam.samples_per_pixel = 100;
    // TODO: make it possible to turn off annealing, since it is computationally very expensive 
    // --> computes samples_per_pixel ray colors for each pixel! Is there a way to reuse the already calculated values?
    cam.max_depth = 50;

    cam.vfov     = 50;
    cam.lookfrom = point3(0,3.5,4);
    cam.lookat   = point3(-1,3,-1);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 2.0; 
    cam.focus_dist    = 3.4;

    cam.render(world);
}

void random_spheres() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    // world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    auto center2 = center + vec3(0, random_double(0,.5), 0); // falling spheres
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
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

    // it is slower with bvh for some reason?
    // world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    //cam.image_width       = 1200;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void two_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(0,0,12);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe));
}

void two_perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>();
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() { // Easy switch between schenes
    switch (5)
    {
    case 1:
        main_schene();
        break;
    case 2:
        random_spheres();
        break;
    case 3:
        two_spheres();
        break;
    case 4:
        earth();
        break;
    case 5:
        two_perlin_spheres();
        break;
    default:
        break;
    }
}