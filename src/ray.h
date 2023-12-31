#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {

    public:
        
        // Constructors
        ray() {}
        ray(const point3& origin, const vec3& direction, double time=0.0) { 
            // when you put "const" in front of a parameter, it means that it cannot be modified in the function
            orig = origin; // or orig(origin)
            dir = direction;
            tm = time;
        }

        // Getters
        point3 origin() const { return orig; } // const after function name means (only applicable for member functions)
        // --> that the function is not allowed to alter the state of the object
        vec3 direction() const { return dir; }
        double time() const { return tm; }

        point3 at(double t) const {
            return orig + t*dir;
        }

    private:
        point3 orig;
        vec3 dir;
        double tm; // we need to add ray time for motion blur

};

#endif