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

#include <iostream>

using namespace std;

// ======================================================
// (OUTDATED) --> now these calculations are defined within generic objects
// double hit_sphere(const point3& center, double radius, const ray& r) { // check if given ray hits a sphere given its center and radius
//     vec3 oc = r.origin() - center; // r.origin is the camera
//     auto a = dot(r.direction(), r.direction()); // we basically see if the ray r=A+tb hits the sphere for a t
//     // --> if we write the equation and solve it for t, we get a quadratic  equation for the defined a, b and c
//     auto b = 2.0 * dot(oc, r.direction()); // you can simplify these equations by considering half_b
//     auto c = dot(oc, oc) - radius*radius;
//     auto discriminant = b*b - 4*a*c;
//     if (discriminant >= 0) { // we don't care if the ray hits once or twice, since we only care about the first time the ray hits sth
//         return (-b-sqrt(discriminant)) / (2.0*a);
//     } else  {
//         return -1.0; 
//     }
// }
// color ray_color(const ray& r) { // we need to calculate the color at our screen
//     auto t = hit_sphere(point3(0,0,-1), 0.5, r); // 0.5 radius makes sense, because remember, our focal_length is 1 and viewport_height is 2!
//     if (t > 0.0) {
//         vec3 N = unit_vector(r.at(t) - point3(0,0,-1)); // normal is the vector from the center to where the ray hits the sphere
//         return 0.5*color(N.x()+1, N.y()+1, N.z()+1); // simple normal coloring scheme
//     }
//     vec3 unit_direction = unit_vector(r.direction()); // --> vec3 unit_direction = r.direction() / r.direction().length();
//     auto a = 0.5*(unit_direction.y() + 1.0); // color changes based on the y-coordinate (y is in [-1,1])
//     return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0); // color scheme, that creates blue-to-white gradient
// }
// ======================================================

// At its core, a ray tracer sends rays through pixels and computes the color seen in the direction of those rays.
int main() {

    // WORLD

    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    auto material_metal_cuboid  = make_shared<lambertian>(color(0.0, 0.125, 1.0));
    world.add(make_shared<cuboid>(point3(-1.0, 0.0, -1.0), 0.8, 0.8, 0.8, material_metal_cuboid));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),  -0.4, material_left)); // use inverted radius to invert their normals
    // --> thus making a hollow glass sphere, if you combine the above 2 surfaces
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -0.5),   0.5, material_right));

    // CAMERA
    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400;
    //cam.image_width  = 1200;
    cam.samples_per_pixel = 100;
    // TODO: make it possible to turn off annealing, since it is computationally very expensive 
    // --> computes samples_per_pixel ray colors for each pixel! Is there a way to reuse the already calculated values?
    cam.max_depth = 50;

    cam.render(world);
}