#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory> // contains shared_ptr
#include <vector>

using std::shared_ptr;
// designed for scenarios in which more than one owner might have to manage the lifetime of the object in memory
// (Note from tutorial) We'll use shared pointers in our code, because it allows multiple geometries to share a common instance 
// --> (for example, a bunch of spheres that all use the same color material), and because it makes memory management 
// --> automatic and easier to reason about.
using std::make_shared;
using namespace std;

class hittable_list : public hittable { // extends hittable, however we use its objects vector

    public:
        vector<shared_ptr<hittable>> objects; // we have a vector of pointers to hittables, not simply hittables

        // Constructors
        hittable_list() {} // empty constructor
        hittable_list(shared_ptr<hittable> object) { add(object); }

        // Functions
        void clear() { objects.clear(); }

        void add(shared_ptr<hittable> object) {
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box()); // extend the bbox, everytime a new object is added.
            // gotta use -> operator, since object is a ptr
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            // This function checks for every object, if the ray hits them. It then fills out the hit_record with the hit information of the
            // --> closest object
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = ray_t.max;

            for(const auto& object  : objects) {
                if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) { // -> is similar to . operator (like hittable.set_face_normal(...))
                // --> the difference is that -> access the members of the structure or the unions using pointers (so object is a pointer)
                // !IMPORTANT: hit object fills out the temp_rec, if there is a hit. So that means at the end we have the hit_record of the
                // --> closest object!
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }
            return hit_anything;
        }

        aabb bounding_box() const override {
            return bbox;
        }

    private:
        aabb bbox;

};


#endif