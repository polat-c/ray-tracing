#ifndef CUBOID_H
#define CUBOID_H

#include "hittable.h"

class cuboid : public hittable { // extends hittable 
    // Start with a simple cuboid --> axis aligned, then add rotation matrices

    public:
        // Constructors
        cuboid(point3 _center, double _x_length, double _y_length, double _z_length, shared_ptr<material> _material) :
         center(_center), x_length(_x_length), y_length(_y_length), z_length(_z_length), mat(_material) {
            alpha, beta, gamma = 0.0, 0.0, 0.0; // aligned cube
            auto rvec = vec3(x_length/2, y_length/2, z_length/2);
            bbox = aabb(center-rvec, center+rvec);
         };

        cuboid(point3 _center, double _x_length, double _y_length, double _z_length, shared_ptr<material> _material,
        double _alpha, double _beta, double _gamma, string _angles) :
         center(_center), x_length(_x_length), y_length(_y_length), z_length(_z_length), mat(_material), 
         alpha(_alpha), beta(_beta), gamma(_gamma), angles(_angles) {
            bbox = aabb(compute_bbox_min(), compute_bbox_max());
         };

        // Functions
        bool hit_aligned(const ray& r, interval ray_t, hit_record& rec, bool rotated=false) const { // overrides abstract class method
            point3 center_transformed;
            if (rotated) {
                center_transformed = point3(0,0,0);
            } else {
                center_transformed = center;
            }
            // this function checks if the ray hits the cuboid, if true, it fills out the hit_record
            interval x_interval = interval(center_transformed.x() - x_length/2, center_transformed.x() + x_length/2);
            interval y_interval = interval(center_transformed.y() - y_length/2, center_transformed.y() + y_length/2);
            interval z_interval = interval(center_transformed.z() - z_length/2, center_transformed.z() + z_length/2);

            double t_tilde = infinity;
            int side; // 1 for x, 2 for y, 3 for z

            // Compute for x-sides
            double t_test_x[2] = {(x_interval.min - r.origin().x()) / r.direction().x(), (x_interval.max - r.origin().x()) / r.direction().x()};
            for (double t_test : t_test_x) {
                if (t_test <= ray_t.min || ray_t.max <= t_test) {
                    continue;
                }
                double check_y = r.at(t_test).y();
                double check_z = r.at(t_test).z();
                if (y_interval.contains(check_y) && z_interval.contains(check_z) && (t_test < t_tilde)) {
                    t_tilde = t_test;
                    side = 1;
                }
            }
            // Compute for y-sides
            double t_test_y[2] = {(y_interval.min - r.origin().y()) / r.direction().y(), (y_interval.max - r.origin().y()) / r.direction().y()};
            for (double t_test : t_test_y) {
                if (t_test <= ray_t.min || ray_t.max <= t_test) {
                    continue;
                }
                double check_x = r.at(t_test).x();
                double check_z = r.at(t_test).z();
                if (x_interval.contains(check_x) && z_interval.contains(check_z) && (t_test < t_tilde)) {
                    t_tilde = t_test;
                    side = 2;
                }
            }
            // Compute for z-sides
            double t_test_z[2] = {(z_interval.min - r.origin().z()) / r.direction().z(), (z_interval.max - r.origin().z()) / r.direction().z()};
            for (double t_test : t_test_z) {
                if (t_test <= ray_t.min || ray_t.max <= t_test) {
                    continue;
                }
                double check_x = r.at(t_test).x();
                double check_y = r.at(t_test).y();
                if (x_interval.contains(check_x) && y_interval.contains(check_y) && (t_test < t_tilde)) {
                    t_tilde = t_test;
                    side = 3;
                }
            }

            if (t_tilde != infinity) {
                rec.t = t_tilde;
                rec.p = r.at(t_tilde);
                vec3 outward_normal;
                switch (side)
                {
                case 1:
                    outward_normal = vec3((rec.p-center_transformed).x() > 0 ? 1.0 : -1.0 , 0.0, 0.0);
                    break;
                case 2:
                    outward_normal = vec3(0.0, (rec.p-center_transformed).y() > 0 ? 1.0 : -1.0 , 0.0);
                    break;
                case 3:
                    outward_normal = vec3(0.0, 0.0, (rec.p-center_transformed).z() > 0 ? 1.0 : -1.0);
                    break;
                }
                rec.set_face_normal(r, outward_normal);
                rec.mat = mat;
                return true;
            } else {
                return false;
            }
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            if ((alpha==0.0) && (beta==0.0) && (gamma==0.0)) { 
                return hit_aligned(r, ray_t, rec); 
            }
            vec3 translation = point3(0,0,0) - center; // calculating the translation vector 
            point3 ray_origin_rotated = rotate3d(r.origin()+translation, alpha, beta, gamma, true, angles);
            // the ray direction is not normalized, thus it is important to keep everything length-invariant
            point3 ray_tail_rotated = rotate3d(r.origin()+translation + r.direction(), alpha, beta, gamma, true, angles); // ray location after 1 t
            ray rotated_ray = ray(ray_origin_rotated, ray_tail_rotated-ray_origin_rotated);

            if (hit_aligned(rotated_ray, ray_t, rec, true)) {
                point3 rotated_p = rotate3d(rec.p, alpha, beta, gamma, false, angles) - translation; // more like transformed_p
                vec3 rotated_normal = rotate3d(rec.p + rec.normal, alpha, beta, gamma, false, angles) - translation - rotated_p;
                rec.p = rotated_p;
                rec.normal = rotated_normal;
                return true;
            } else {
                return false;
            }
        }

        aabb bounding_box() const override {
            return bbox;
        }

    private:
        point3 center;
        double x_length, y_length, z_length;
        double alpha, beta, gamma; // alignment angles of the cuboid
        string angles;
        shared_ptr<material> mat;
        aabb bbox;

        int bbox_loop_array[8][3] = {{1,1,1}, {1,1,-1}, {1, -1, -1}, {1, -1, 1}, 
            {-1, 1, 1}, {-1, 1, -1}, {-1, -1, -1}, {-1, -1, 1}};
        point3 compute_bbox_min() const {
            double min_x, min_y, min_z = infinity;
            point3 considered_vertex;

            for(int i=0; i<8; i++) {;
                considered_vertex = center + 0.5*point3(bbox_loop_array[i][0]*x_length, 
                    bbox_loop_array[i][1]*y_length, 
                    bbox_loop_array[i][2]*z_length);
                considered_vertex = rotate3d(considered_vertex, alpha, beta, gamma);

                if (considered_vertex.x() < min_x) min_x = considered_vertex.x();
                if (considered_vertex.y() < min_y) min_y = considered_vertex.y();
                if (considered_vertex.z() < min_z) min_z = considered_vertex.z();
            }
            return point3(min_x, min_y, min_z);
        }
        point3 compute_bbox_max() const {
            double max_x, max_y, max_z = -infinity;
            point3 considered_vertex;

            for(int i=0; i<8; i++) {;
                considered_vertex = center + 0.5*point3(bbox_loop_array[i][0]*x_length, 
                    bbox_loop_array[i][1]*y_length, 
                    bbox_loop_array[i][2]*z_length);
                considered_vertex = rotate3d(considered_vertex, alpha, beta, gamma);

                if (considered_vertex.x() > max_x) max_x = considered_vertex.x();
                if (considered_vertex.y() > max_y) max_y = considered_vertex.y();
                if (considered_vertex.z() > max_z) max_z = considered_vertex.z();
            }
            return point3(max_x, max_y, max_z);
        }

};

#endif