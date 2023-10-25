#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>
const double infinity = std::numeric_limits<double>::infinity();

class interval {

    public:
        double min, max;

        // Constructor
        interval() : min(+infinity), max(-infinity) {} // Default interval is empty
        interval(double _min, double _max) : min(_min), max(_max) {}

        // Functions
        bool contains(double x) {
            return (min <= x) && (x <= max);
        }

        bool surrounds(double x) {
            return (min < x) && (x < max);
        }

        double clamp(double x) const {
            if(x < min) return min;
            if(max < x) return max;
            return x;
        }

        static const interval empty, universe; // generic intervals
};

const static interval empty(+infinity, -infinity);
const static interval universe(-infinity, +infinity);

#endif