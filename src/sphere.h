#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable { // extends hittable 

    public:
        // Constructors
        // Stationary Sphere
        sphere(point3 _center, double _radius, shared_ptr<material> _material) : center1(_center), radius(_radius),
         mat(_material), is_moving(false) {}; // constructor

        // Moving Sphere
        sphere(point3 _center1, point3 _center2, double _radius, shared_ptr<material> _material) : center1(_center1), radius(_radius),
         mat(_material), is_moving(true) {
            center_vec = _center2 - _center1;
         }

        // Functions
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override { // overrides abstract class method
            // this function checks if the ray hits the sphere, if true, it fills out the hit_record
            point3 center = is_moving ? sphere_center(r.time()) : center1; // calculating center loc for given time-point, for moving spheres
            vec3 oc = r.origin() - center;
            auto a = r.direction().length_squared();
            auto half_b = dot(oc, r.direction());
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = half_b*half_b - a*c;
            if (discriminant < 0) return false; // returns false if the quadratic equation does not have a solution, 
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrtd) / a;
            if (root <= ray_t.min || ray_t.max <= root) { // if (!ray_t.surrounds(root))
                root = (-half_b + sqrtd) / a;
                if (root <= ray_t.min || ray_t.max <= root) {
                    return false;
                }
            }
            
            rec.t = root;
            rec.p = r.at(root);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat;

            return true;
        }

    private:
        point3 center1;
        double radius;
        shared_ptr<material> mat;
        bool is_moving;
        vec3 center_vec;

        point3 sphere_center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0 yields
            // center1, and t=1 yields center2.
            return center1 + time * center_vec;
        }
};

#endif