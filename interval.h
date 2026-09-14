#ifndef INTERVAL_H
#define INTERVAL_H
#include "rtweekend.h"

class interval {
    public:
    double min, max;

    interval () : min (+infinity), max (-infinity) {}
    interval (const double& min, const double& max) : min (min), max (max) {}

    double size() const { return max - min;}
    double contains (const double& x) const { return min <= x && x <= max;}
    double surrounds (const double& x) const { return min < x && x < max;}
    double clamp (const double& x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
    static const interval empty, universe;
};

const interval interval::empty = interval (+infinity, +infinity);
const interval interval::universe = interval (-infinity, -infinity);

#endif //INTERVAL_H
