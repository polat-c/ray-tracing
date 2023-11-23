#ifndef QUAD_H
#define QUAD_H

#include <cmath>
#include "hittable.h"

class quad : public hittable { // 2D object defined by base Q, and vectors u & v

    public:
        // Constructors
        quad(const point3 &_Q, const vec3 &_u, const vec3 &_v, shared_ptr<material> m) 
            : Q(_Q), u(_u), v(_v) , mat(m) {
            // Check the Ray-Plane Intersection Section --> Very easy & trivial calculations
            auto n = cross(u, v);
            normal = unit_vector(n);
            D = dot(normal, Q);
            w = n / dot(n, n);

            set_bounding_box();
        }

        // Functions
        virtual void set_bounding_box() { // abstract method, gotta define specific quads
            bbox = aabb(Q, Q + u + v).pad();
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            auto denom = dot(normal, r.direction());

            // No hit if the ray is parallel to the plane.
            if (fabs(denom) < 1e-8) // abs for floats
                return false;

            // Return false if the hit point parameter t is outside the ray interval.
            auto t = (D - dot(normal, r.origin())) / denom;
            if (!ray_t.contains(t))
                return false;

            // Determine the hit point lies within the planar shape using its plane coordinates.
            auto intersection = r.at(t);
            vec3 planar_hitpt_vector = intersection - Q;
            auto alpha = dot(w, cross(planar_hitpt_vector, v));
            auto beta = dot(w, cross(u, planar_hitpt_vector));

            if (!is_interior(alpha, beta, rec))
                return false;

            rec.t = t;
            rec.p = intersection;
            rec.mat = mat;
            rec.set_face_normal(r, normal);

            return true;
        }

        virtual bool is_interior(double a, double b, hit_record& rec) const {
            // --> Only function to change for arbitrary 2D objects --> now it's way easier to implement triagles
            // Given the hit point in plane coordinates, return false if it is outside the
            // primitive, otherwise set the hit record UV coordinates and return true.

            if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

            rec.u = a; // setting hit coordinates for texture mapping
            rec.v = b;
            return true;
        }

        aabb bounding_box() const override {
            return bbox;
        }

    private:
        point3 Q;
        vec3 u, v;
        shared_ptr<material> mat;
        aabb bbox;
        vec3 normal;
        double D;
        vec3 w;

};

#endif