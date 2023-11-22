#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.h"

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

#endif