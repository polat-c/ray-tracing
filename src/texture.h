#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"
#include "rt_stb_image.h"
#include "perlin.h"

class texture { // Abstract class
    public:
        // Destructor
        virtual ~texture() = default;

        virtual color value(double u, double v, const point3& p) const = 0;
        // u and v are the surface coordinates
};

class solid_color : public texture { // Single Color texture

    public:
        // Constructors
        solid_color(color c) : color_value(c) {}

        solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

        // Functions
        color value(double u, double v, const point3& p) const override {
            return color_value;
        }

    private:
        color color_value;

};

class checker_texture : public texture { // This is a spatial texture --> depends only on the point in 3D space
// --> no relation with the object

    public:
        // Constructors
        checker_texture(double _scale, shared_ptr<texture> _even, shared_ptr<texture> _odd) : inv_scale(1.0 / _scale), 
            even(_even), odd(_odd) {}
        
        checker_texture(double _scale, color c1, color c2) : inv_scale(1.0 / _scale), 
            even(make_shared<solid_color>(c1)),
            odd(make_shared<solid_color>(c2)) {}

        // Functions
        color value(double u, double v, const point3& p) const override {
            auto xInteger = static_cast<int>(std::floor(inv_scale * p.x())); // taking the floor funtion, st. we can apply the modulo operator
            auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
            auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

            bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }

    private:
        double inv_scale; // controls the size of the checker pattern
        shared_ptr<texture> even; // two solid_color (not necessarily) textures combined
        shared_ptr<texture> odd;

};

class image_texture : public texture {

    public:
        image_texture(const char* filename) : image(filename) {}

        color value(double u, double v, const point3& p) const override {
            // If we have no texture data, then return solid cyan as a debugging aid.
            if (image.height() <= 0) return color(0,1,1);

            // Clamp input texture coordinates to [0,1] x [1,0]
            u = interval(0,1).clamp(u); // this does not change the scale, just clamps the outside values
            v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

            auto i = static_cast<int>(u * image.width()); // Scaling to image pixel sizes
            auto j = static_cast<int>(v * image.height()); // --> exploit functions defined in rt_image class
            auto pixel = image.pixel_data(i,j);

            auto color_scale = 1.0 / 255.0; // Our camera uses color scale [0,1], not [0, 255]
            return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
        }

    private:
        rt_image image;

};

class noise_texture : public texture {

    public:
        noise_texture() {}
        // Our perlin noise is defined via static methods
        // --> doesn't depend on texture parametes, can be called directly

        color value(double u, double v, const point3& p) const override {
            return color(1,1,1) * noise.noise(p);
        }

    private:
        perlin noise; // our noise generator --> class generated upon initialization

};

#endif