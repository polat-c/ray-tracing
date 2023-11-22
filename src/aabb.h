#ifndef AABB_H
#define AABB_H

#include "general.h"
#include "ray.h"

class aabb { // axis-aligned bounding boxes

    public:
        interval x, y, z;

        // Constructors
        aabb() {} // default aabb is empty

        aabb(const interval& ix, const interval& iy, const interval& iz) : x(ix), y(iy), z(iz) {}

        aabb(const point3& a, const point3& b) {
            // Treat the two points a and b as extrema for the bounding box, so we don't require a
            // particular minimum/maximum coordinate order.
            x = interval(fmin(a[0],b[0]), fmax(a[0],b[0]));
            y = interval(fmin(a[1],b[1]), fmax(a[1],b[1]));
            z = interval(fmin(a[2],b[2]), fmax(a[2],b[2]));
        }

        aabb(const aabb& bbox1, const aabb& bbox2) {
            x = interval(bbox1.x, bbox2.x);
            y = interval(bbox1.y, bbox2.y);
            z = interval(bbox1.z, bbox2.z);
        }

        // Functions
        const interval& axis(int n) const { // util function, so it is possible to use for loops
            if (n==1) return y;
            if (n==2) return z;
            return x;
        }

        bool hit(const ray& r, interval ray_t) const { // ray_t will be modified within the function, thus not const
            // Hit occurs, if the hits of different t intervals overlap!!!
            for(int a=0; a<3; a++) {
                // fmin & fmax can be used with floats, min & max are the safe choice for any object with <(>) implemented
                auto t0 = min((axis(a).min - r.origin()[a]) / r.direction()[a], // just like the implementation in cuboid
                           (axis(a).max - r.origin()[a]) / r.direction()[a]);
                auto t1 = max((axis(a).min - r.origin()[a]) / r.direction()[a],
                            (axis(a).max - r.origin()[a]) / r.direction()[a]);
                // Below lines are calculating, if the intervals overlap. The interval ray_t is shortened
                // with the calculated t-values for xyz-intervals. If the ray_t interval vanishes (st. ray_t.max <= ray_t.min),
                // that means that the intervals are not overlapping!
                ray_t.min = max(t0, ray_t.min);
                ray_t.max = min(t1, ray_t.max);
                if (ray_t.max <= ray_t.min)
                    return false;
            }
            return true;
        }

        // --> Optimized Hit-Method
        // bool hit(const ray& r, interval ray_t) const {
        //     for (int a = 0; a < 3; a++) {
        //         auto invD = 1 / r.direction()[a];
        //         auto orig = r.origin()[a];

        //         auto t0 = (axis(a).min - orig) * invD;
        //         auto t1 = (axis(a).max - orig) * invD;

        //         if (invD < 0)
        //             std::swap(t0, t1);

        //         if (t0 > ray_t.min) ray_t.min = t0;
        //         if (t1 < ray_t.max) ray_t.max = t1;

        //         if (ray_t.max <= ray_t.min)
        //             return false;
        //     }
        //     return true;
        // }

};

#endif