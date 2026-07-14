#include "main_header.h"

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include <chrono>


using namespace std;



int main() {
    clog << "Hello, Ray Tracer!\n";
    auto start = chrono::high_resolution_clock::now();

    hittable_list world;

    //auto R = std::cos(pi/4);

    auto ground_material = make_shared<lambertian>(color{0.5f, 0.5f, 0.5f});
    world.add(make_shared<sphere>(point3{0.0f,-1000.0f,0.0f}, 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center{a + 0.9f*random_float(), 0.2f, b + 0.9f*random_float()};

            if (length(center - point3{4.0f, 0.2f, 0.0f}) > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec() * random_vec();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec(0.5f, 1.0f);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3{0.0f, 1.0f, 0.0f}, 1.0, material1));

    auto material2 = make_shared<lambertian>(color{0.4f, 0.2f, 0.1f});
    world.add(make_shared<sphere>(point3{-4.0f, 1.0f, 0.0f}, 1.0, material2));

    auto material3 = make_shared<metal>(color{0.7f, 0.6f, 0.5f}, 0.0);
    world.add(make_shared<sphere>(point3{4.0f, 1.0f, 0.0f}, 1.0, material3));


    //world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    //world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.width  = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50; 
    cam.vfov = 20;
    cam.lookfrom = point3{13.0f, 2.0f, 3.0f};
    cam.lookat = point3{0.0f, 0.0f, 0.0f};
    cam.vup = vec3{0.0f,1.0f,0.0f};
    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render(world);
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<std::chrono::seconds>(end - start);
    clog << "Render completed in " << duration.count() << " seconds.\n";

}