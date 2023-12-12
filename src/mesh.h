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

#endif