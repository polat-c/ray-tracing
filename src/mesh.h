#ifndef MESH_H
#define MESH_H

#include "quad.h"

class triangle : public quad {

    public:
        using quad::quad;
        using quad::set_bounding_box;
        using quad::hit;
        using quad::bounding_box;

        bool is_interior(double a, double b, hit_record& rec) const {
            if ((a < 0) || (b < 0) || (1 < a+b))
                return false;

            rec.u = a;
            rec.v = b;
            return true;
        }

};

class mesh {

    public:
        vector<point3> vertices;
        vector<unsigned int> vindices;

        bool create_object(hittable_list& object, shared_ptr<material> mat, double scale=1) {
            for(int i=0; i<vindices.size(); i++) {
                point3 Q = (vertices[vindices[i]-1]) * scale;
                vec3 u = (vertices[vindices[i+1]-1] - vertices[vindices[i]-1]) * scale;
                vec3 v = (vertices[vindices[i+2]-1] - vertices[vindices[i]-1]) * scale;
                object.add(make_shared<triangle>(Q, u, v, mat));
                i += 2;
            }
            return true;
        }
};

#endif