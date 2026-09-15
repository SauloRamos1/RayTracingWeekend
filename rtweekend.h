#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <limits>
#include <memory>
#include <random>

// C++ std usings
using std::make_shared;
using std::shared_ptr;

// Constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility functions
inline double degrees_to_radians(const double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double() {
    // thread_local: each render thread owns its generator (no data race under std::execution::par)
    thread_local std::mt19937 gen(std::random_device{}());
    thread_local std::uniform_real_distribution<double> dis(0.0, 1.0);
    return dis(gen);
}

inline double random_double(const double min, const double max) {
    return min + (max - min) * random_double();
}

// Common Headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif //RTWEEKEND_H
