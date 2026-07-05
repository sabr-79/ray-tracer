#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include "main_header.h"
#include "interval.h"
#include <iostream>
using namespace std;

using color = vec3;

inline double linear_to_gamma(double lin_component){
    if (lin_component > 0){
        return sqrt(lin_component);
    }
    return 0;
}

void write_color(ostream& out, const color& pixel_color){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // applying lin to gamma transform for gamma 2 

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const interval intensity(0.000, 0.999);
    
    // antialiasing: translating [0, 1] component vals into byte ranges
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    
    // write pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';


}

#endif
