#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable { // extends hittable 

    public:
        sphere(point3 _center, double _radius, shared_ptr<material> _material) : center(_center), radius(_radius), mat(_material) {}; // constructor

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override { // overrides abstract class method
            // this function checks if the ray hits the sphere, if true, it fills out the hit_record
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
        point3 center;
        double radius;
        shared_ptr<material> mat;

};

#endif