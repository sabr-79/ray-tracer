#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include "main_header.h"
#include "interval.h"
#include <iostream>
using namespace std;

using color = vec3;

void write_color(ostream& out, const color& pixel_color){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    static const interval intensity(0.000, 0.999);
    
    // antialiasing: translating [0, 1] component vals into byte ranges
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    
    // write pixel color components
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';


}

#endif
