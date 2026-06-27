#include <iostream>
#include "vec3.h"
#include "color.h"
using namespace std;

int main() {
   clog << "Hello, Ray Tracer!\n";
    

    int width = 256;
    int height = 256;

    cout << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; j++){ 
        //clog << "Scanlines remaining: " << (height - j) << "\n";
        for (int i = 0; i < width; i++){
            auto pixel_color = color(double(i)/(width-1), 0, double(j)/(height-1));
            write_color(cout, pixel_color);
        }
    }
    clog << "\rDone.                 \n"; 
}