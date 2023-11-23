#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"

class material;

class hit_record {
    public:
        point3 p; // all of these attributes are filled out within a hit() function
        vec3 normal;
        shared_ptr<material> mat;
        double t;
        double u; // surface coordinates / hit point p is not enough to map it to the texture
        double v;
        bool front_face; // save the information, if the ray hits the object from the front or the back

        void set_face_normal(const ray& r, const vec3& outward_normal) {
            // Sets the hit record normal vector.
            // NOTE: the parameter `outward_normal` is assumed to have unit length.

            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal; // !IMPORTANT: we set our normals s.t they point against the ray
        }
};

class hittable { // our abstract class
    public:
        // A virtual function (also known as virtual methods) is a member function 
        // that is declared within a base class and is re-defined (overridden) by a derived class.
        virtual ~hittable() = default; // our abstract method (virtual), defines the default destructor(~)

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
        // this function checks if the ray hits the sphere, if true, it fills out the hit_record
        // --> ray_t is not const reference, since it is manipulated in aabb hit function (to simplify the calculations)

        virtual aabb bounding_box() const = 0;
};

#endif