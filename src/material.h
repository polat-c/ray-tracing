#ifndef MATERIAL_H
#define MATERIAL_H

#include "general.h"
#include "color.h"
#include "hittable.h"

class material { // abstract class

    public:
        virtual ~material() = default;

        virtual bool scatter(
            const ray& r_in,
            const hit_record& rec,
            color& attenuation,
            ray& scattered
            ) const = 0;
};

class lambertian : public material { // type of surface (calculations based on reflectance), we the light scatters everytime for simplicity

    public:
        // Constructors
        lambertian(const color& a) : albedo(a) {}

        // Functions
        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override { 
            // fills out the attenuation & scattered
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero()) // there is a possibility that random unit vector is exactly -normal, 
                // this check prevents any zeros there
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    private:
        color albedo;
};

class metal : public material { // reflects ligth with the incoming angle (vec3.reflect)

    public:
        // Constructors
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        // Functions
        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            auto reflected = reflect(unit_vector(r_in.direction()), rec.normal); // I don't know if unit_vector is needed
            scattered = ray(rec.p, reflected + fuzz*random_unit_vector());
            attenuation = albedo;
            return (dot(rec.normal, scattered.direction()) > 0); // if normal and scattered is not in the same direction, that means that
            // --> the scattered lies within the object, so object absorbs the light!
        }

    private:
        color albedo;
        double fuzz;
};

class dielectric : public material { // refracts all light
    
    public:
        // Constructors
        dielectric(double _ir) : ir(_ir) {}

        // Functions
        bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            attenuation = color(1.0, 1.0, 1.0); // we do not have a color for dielectrics
            // IMPORTANT: it is important if the ray hits the surface from front or back (from inside the object)
            // --> if the latter is true, the refractive index is inverted
            double refraction_ratio = (rec.front_face) ? (1/ir) : ir; // we assume the refractive index of the environment to be =1

            // All Physics, determining if the light is reflected or refracted
            vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) 
                // it reflects, if it cannot refract OR the angle is so large
                // --> the object has more tendency to reflect the larger the incident angle is
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction);
            return true;
        }

    private:
        double ir; // Index of Refraction

        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

#endif