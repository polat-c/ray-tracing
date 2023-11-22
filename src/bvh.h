// BVH --> Bounding Volume Hierarchies
#ifndef BVH_H
#define BVH_H

#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable {

    public:
        // Constructors
        bvh_node(const hittable_list& list) : bvh_node(list.objects, 0, list.objects.size()) {} 

        bvh_node(const std::vector<shared_ptr<hittable>>& src_objects, size_t start, size_t end) {
            // size_t is basically like an int that represents sizes
            auto objects = src_objects;

            // Our strategy: choose random axix, sort objects based on that axis, split the left&right bvhs based on that object
            int axis = random_int(0,2);
            auto comparator = (axis==0) ? box_x_compare : // choosing our comparator based on the randomly generated axis
                (axis==1) ? box_y_compare : box_z_compare;

            size_t object_span = end - start; // amount of objects

            if (object_span == 1) {
                // our strategy for a single object: dupe left&right with the same object
                left = right = objects[start];
            } else if (object_span == 2 ) {
                // sort first!
                if (comparator(objects[start], objects[start+1])) {
                    left = objects[start];
                    right = objects[start+1];
                } else {
                    left = objects[start];
                    right = objects[start+1];
                }
            } else {
                // sort first!
                // --> sort method sorts the vector based on the given comparator
                // --> .begin() functions returns an iterator to the beginning of the sequence
                // --> it is important that the objects are not const qualified so they can be sorted!
                std::sort(objects.begin()+start, objects.begin()+end, comparator);
                auto mid = start + object_span / 2;
                // we do not have to define make_shared<hittable>, since we already covered 1 and 2 object scenarios
                left = make_shared<bvh_node>(objects, start, mid);
                right = make_shared<bvh_node>(objects, mid, end);
            }
        }

        // Functions
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            if (!bbox.hit(r, ray_t)) // aabb hit function does not have rec as parameter
                return false;
            bool hit_left = left->hit(r, ray_t, rec);
            // before reaching this line, every object on the left bvh_node have been considered!
            // --> so if there is a hit on the left node, we already filled out rec, thus we only
            // --> need to consider if an object is hit in the right node BEFORE hitting the object in the left node
            // --> thus we can make the ray_t interval smaller
            bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);
            return hit_left || hit_right;
        }

        aabb bounding_box() const override {return bbox;}

    private:
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        aabb bbox;

        // Box comparator functions
        static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis_index) {
            // check which interval has the smaller min value
            return a->bounding_box().axis(axis_index).min < b->bounding_box().axis(axis_index).min;
        }
        static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 0);
        }
        static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 1);
        }
        static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
            return box_compare(a, b, 2);
        }

};

#endif