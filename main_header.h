#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

using namespace std;

// Constants

const double infinity = numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// thread safe for gcd
inline double random_double() {
    // returns random real [0, 1)
    static thread_local mt19937 rng(random_device{}());
    static thread_local uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

inline double random_double(double min, double max) {
    // returns random real [min, max)
    return min + (max-min) * random_double();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    static thread_local mt19937 rng(std::random_device{}());
    uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Common Headers

#include "color.h"
#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "bvh.h"
#endif