#ifndef PERLIN_H
#define PERLIN_H

#include "general.h"

// Helper class to perlin noise texture
// --> Perlin Noise theory: https://en.wikipedia.org/wiki/Perlin_noise
class perlin {
  public:
    // Constructors
    perlin() {
        ranfloat = new double[point_count];
        for (int i = 0; i < point_count; ++i) {
            ranfloat[i] = random_double();
        }

        perm_x = perlin_generate_perm(); // random permutation of [0, 1, 2, ... , point_count-1]
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    // Destructor
    ~perlin() {
        delete[] ranfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    // Functions
    double noise(const point3& p) const { // Our main noise generation function
        auto i = static_cast<int>(4*p.x()) & 255; // & is bitwise AND operator
        auto j = static_cast<int>(4*p.y()) & 255;
        auto k = static_cast<int>(4*p.z()) & 255;

        return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]]; // ^ is bitwise XOR operator
    }

  private:
    static const int point_count = 256;
    double* ranfloat;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm() {
        auto p = new int[point_count];

        for (int i = 0; i < perlin::point_count; i++) // :: is the scope resolution operator 
            // --> used to define the class or the namespace
            p[i] = i;

        permute(p, point_count);

        return p;
    }

    static void permute(int* p, int n) {
        for (int i = n-1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
};

#endif