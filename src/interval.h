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
        interval(const interval& a, const interval& b) {
            min = fmin(a.min, b.min); // float min & max
            max = fmax(a.max, b.max);
        }

        // Functions
        bool contains(double x) const { // interval cannot be changed with these functions
            return (min <= x) && (x <= max);
        }

        bool surrounds(double x) const {
            return (min < x) && (x < max);
        }

        double clamp(double x) const {
            if(x < min) return min;
            if(max < x) return max;
            return x;
        }

        double size() const {
            return max-min;
        }

        interval expand(double delta) const {
            auto padding = delta/2;
            return interval(min-padding, max+padding);
        }

        static const interval empty, universe; // generic intervals
};

const static interval empty(+infinity, -infinity);
const static interval universe(-infinity, +infinity);

interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
};

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
};

#endif