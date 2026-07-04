#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"
#include "main_header.h"
#include <iostream>
using namespace std;

using color = vec3;

void write_color(ostream& out, const color& pixel_color){
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';


}

#endif
