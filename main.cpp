#include <iostream>
using namespace std;

int main() {
   //cout << "Hello, Ray Tracer!" << endl;
    

    int width = 256;
    int height = 256;

    cout << "P3\n" << width << ' ' << height << "\n255\n";

    for (int j = 0; j < height; j++){
        for (int i = 0; i < width; i++){
            auto r = double(i) / (width -1);
            auto g = double(j) / (height-1);
            auto b = 0.0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    
}