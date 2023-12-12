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

class translate : public hittable {
    
    public:
        // Constructors
        translate(shared_ptr<hittable> obj, const vec3& offs) : object(obj), offset(offs) {
            bbox = object->bounding_box() + offset;
        }

        // Functions
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            ray translated_r = ray(r.origin()-offset, r.direction(), r.time());
            if (!object->hit(translated_r, ray_t, rec)) {
                return false;
            }

            rec.p += offset;
            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        shared_ptr<hittable> object;
        vec3 offset;
        aabb bbox;
};

class rotate_y : public hittable {
    // rotation about y-axis, our cuboid is rotated about x,y and z axes, but this is a more generic method for every object
    public:
        // Constructors
        rotate_y(shared_ptr<hittable> obj, double angle) : object(obj) {
            auto radians = degrees_to_radians(angle);
            sin_theta = sin(radians);
            cos_theta = cos(radians);
            bbox = object->bounding_box();

            // Adjusting the bounding box --> considering all (8) vertices to find extremal (min, max) points
            point3 min( infinity,  infinity,  infinity);
            point3 max(-infinity, -infinity, -infinity);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        // consider all vertices
                        auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                        auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                        auto z = k*bbox.z.max + (1-k)*bbox.z.min;
                        // rotate them
                        auto newx =  cos_theta*x + sin_theta*z;
                        auto newz = -sin_theta*x + cos_theta*z;

                        vec3 tester(newx, y, newz);
                        // check if the rotated vertex has the min or the max x, y or z value
                        for (int c = 0; c < 3; c++) {
                            min[c] = fmin(min[c], tester[c]);
                            max[c] = fmax(max[c], tester[c]);
                        }
                    }
                }
            }

            bbox = aabb(min, max);
        }

        // Functions
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // Transform the ray from world space to object space with rotation matrices
            auto origin = r.origin();
            auto direction = r.direction();

            origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
            origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];
            // vec3 and point3 transformed identically
            direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
            direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

            ray rotated_r(origin, direction, r.time());

            // Apply hit funtion in object space
            if(!object->hit(rotated_r, ray_t, rec)) {
                return false;
            }

            // Transform hit record to world space
            auto p = rec.p; // hit point
            p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
            p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

            auto normal = rec.normal; // normal
            normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
            normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

            rec.p = p;
            rec.normal = normal;
            return true;
        }

        aabb bounding_box() const override { return bbox; }

    private:
        shared_ptr<hittable> object;
        double sin_theta;
        double cos_theta;
        aabb bbox;
};

#endif