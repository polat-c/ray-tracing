#ifndef CAMERA_H
#define CAMERA_H

#include "general.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"

#include <iostream>
using namespace std;

class camera {

    public:
        double aspect_ratio = 1.0;  // Ratio of image width over height
        int image_width  = 100;  // Rendered image width in pixel count
        int samples_per_pixel = 10; // Count of the random samples for each pixel (for annealing)
        int max_depth = 10;   // Maximum number of ray bounces into scene

        double vfov = 90;  // Vertical view angle (field of view), thanks to this, changing the focal_point does not change the image

        void render(const hittable& world) {
            initialize();

            // RENDER (to ppm format)

            cout << "P3\n" << image_width << " " << image_height << "\n255\n";

            for(int j=0; j<image_height; ++j) {
                //clog << "\rScanlines remaining: " << (image_height-j) << " " << flush;
                clog << "\rRendering... " << static_cast<int>(100*(j+1)/image_height) << "% " << flush;
                for(int i=0; i<image_width; ++i) {
                    // color pixel_color = color(double(i)/(image_width-1), double(j)/(image_height-1), 0); // creating an easy image
                    color pixel_color = color(0,0,0); // or simply color pixel_color(0,0,0)
                    for(int sample=0; sample<samples_per_pixel; ++sample) {
                        ray r = get_ray(i, j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                    write_color(cout, pixel_color, samples_per_pixel);
                }
            }
            clog << "\nFinished.         \n";

        }

    private:
        int    image_height;   // Rendered image height
        point3 center;         // Camera center
        point3 pixel00_loc;    // Location of pixel 0, 0
        vec3   pixel_delta_u;  // Offset to pixel to the right
        vec3   pixel_delta_v;  // Offset to pixel below

        // IMPORTANT
        // --> viewport is just defines the area we are looking at! It is not a screen-like object!

        void initialize() {
            // Calculate the image height, and ensure that it's at least 1.
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height; // set image_height to 1 if image_height < 1, keep it otherwise

            center = point3(0, 0, 1);

            auto focal_length = 1.0; // distance to viewport
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2 * h * focal_length; // we calculate viewport h based on fov, so the fov does not change when we change the focal length
            // auto viewport_height = 2.0; // viewport can assume real values, image can't
            auto viewport_width = viewport_height * (static_cast<double>(image_width)/image_height); // image_width / image_height is not exactly same as aspect_ratio

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            auto viewport_u = vec3(viewport_width, 0, 0); // horizontal
            auto viewport_v = vec3(0, -viewport_height, 0); // vertical
            
            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = center - viewport_u/2 - viewport_v/2 - point3(0, 0, focal_length);
            pixel00_loc = viewport_upper_left + pixel_delta_u/2 + pixel_delta_v/2;
        }

        ray get_ray(int i, int j) {
            // Get a randomly sampled camera ray for the pixel at location i,j.

            auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_sample_square();

            auto ray_origin = center;
            auto ray_direction = pixel_sample - ray_origin; // ray going from camera to image
            return ray(ray_origin, ray_direction);
        }

        point3 pixel_sample_square() const {
            // Returns a random point in the square surrounding a pixel at the origin.
            auto px = -0.5 + random_double(); // this seems like we are sampling a point within the bottom-left part of the pixel tho
            auto py = -0.5 + random_double();
            return (px * pixel_delta_u) + (py * pixel_delta_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const { // world is defined to be hittable, but since hittable_list
            // --> extends hittable it can also be hittable_list
            hit_record rec;

            // If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0) return color(0,0,0); // should be black like the bottom side of objects

            if(world.hit(r, interval(0.001, infinity), rec)) { // solving shadow acne problem by setting min t0 0.001 instead of 0
                ray scattered;
                color attenuation;
                if (rec.mat->scatter(r, rec, attenuation, scattered)) { // this if is important, if scatter is false (this might be false
                    // --> for metal objects due to fuzzyness), we want the object to absorb all light, making it color(0,0,0)
                    return attenuation * ray_color(scattered, depth-1, world); // attenuation is a color!
                }
                return color(0,0,0);
            }

            vec3 unit_direction = unit_vector(r.direction()); // --> vec3 unit_direction = r.direction() / r.direction().length();
            auto a = 0.5*(unit_direction.y() + 1.0); // color changes based on the y-coordinate (y is in [-1,1])
            return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0); // color scheme, that creates blue-to-white gradient
        }

};

#endif