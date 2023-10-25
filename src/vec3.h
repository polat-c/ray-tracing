// C function declarations
#ifndef VEC3_H
#define VEC3_H
// above 2 are #include guards, preventing same code to be included more than once
// --> https://stackoverflow.com/questions/1653958/why-are-ifndef-and-define-used-in-c-header-files

#include <cmath>
#include <iostream>
#include "general.h"

using namespace std;
using std::sqrt;

class vec3 {

    public: // we need everything public

        double e[3];

        // Constructors
        vec3() : e{0,0,0} {} // empty constructor (: is a way to set e, you can do the same as follows:)
        // vec3() {
        //     e[0]=0;
        //     e[1]=0;
        //     e[2]=0;
        // }
        vec3(double e1, double e2, double e3) : e{e1, e2, e3} {}

        double x() const { return e[0]; } // X-coordinates <== The implicit "this" pointer is const-qualified!
        double y() const { return e[1]; } // Y-coordinates 
        double z() const { return e[2]; } // Z-coordinates 

        // Operator overloading
        // https://stackoverflow.com/questions/60251681/why-is-a-reference-needed-in-operator-overloading 
        // --> why & (reference) is needed for operator overloading

        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]) ;}
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3& v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator/=(double t) {
            return *this *= (1/t);
        }

        double length() const {
            return sqrt(length_squared());
        }

        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions.
            auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        }

        static vec3 random() { // static means that we do not need an instance of this class
            return vec3(random_double(), random_double(), random_double());
        } 

        static vec3 random(double min, double max) { // overloading
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using point3 = vec3; // point3 contains the (x,y,z) coordinates, vec3 contains the directions!

// Utility functions
// inline functions --> https://www.geeksforgeeks.org/inline-functions-cpp/ 

// overload << ostream operator in cout
inline ostream& operator<<(ostream& out, const vec3& v) {
    return out << v.e[0] << " " << v.e[1] << " " << v.e[2]; // could also be written via v.x() instead of v.e[0]
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

// // Using the rejection method
// inline vec3 random_in_unit_sphere() {
//     while (true) {
//         auto p = vec3::random(-1, 1); // you use :: when you want to use the static functions directly from class name.
//         if (p.length_squared() < 1) {
//             return p;
//         }
//     }
// }
// inline vec3 random_unit_vector() {
//     return unit_vector(random_in_unit_sphere()); // why do we need a vector within the unit sphere if we gonna normalize it?
// }

// Just using spherical coordinates
inline vec3 random_unit_vector() {
    auto theta = random_double(0.0, pi);
    auto phi = random_double(0.0, 2*pi);
    return vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
}

inline vec3 random_on_hemisphere(const vec3& normal) { // Last part of random reflection
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) { // same direction
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n) { // calculates the reflected ray from a mirror surface (using vector math)
    // remember, the rays reflected from mirrors are not randomly distributed
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& v, const vec3& n, double etai_over_etat) { // needed for dielectrics, etai is the outside dielectric constant
    // vec3 R_perpendicular = etai_over_etat * (v + (dot(-v, n) * n)); // this was my implementation, apperantly you gotta be careful
    // vec3 R_parallel = - sqrt(1 - R_perpendicular.length_squared()) * n; // --> with the negatives
    // return R_perpendicular + R_parallel;
    auto cos_theta = fmin(dot(-v, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (v + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif