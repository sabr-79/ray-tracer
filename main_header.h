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

const double infinity = 1e30;
const double pi = 3.1415926535897932385;
static thread_local uint32_t rng_state = []{
    random_device rd;
    return rd();
}();

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// thread safe for gcd
inline double random_double() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return (double)rng_state / 4294967296.0;
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