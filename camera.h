#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"
#include <thread>
using namespace std;

class camera {
    public:
        double aspect_ratio = 1.0; // Ratio of image width over height
        int width = 100; // Rendered image width in pixel count
        int samples_per_pixel = 10;   // Count of random samples for each pixel
        int max_depth = 10; // max num of ray bounces into scene
        double vfov = 90; // vertical view angle 
        point3 lookfrom = point3{0,0,0}; // Where cam is looking from
        point3 lookat = point3{0,0,0}; // Where the scene is
        vec3 vup = vec3{0,1,0}; // Camera's relative up direction
        double defocus_angle = 0; // variation angle of rays thru each pixel
        double focus_dist = 10; // dist between lookfrom point to plane of perfect focus



        void render(const hittable& world) {
            initialize();

            vector<color> framebuffer(width * height);
            cout << "P3\n" << width << " " << height << "\n255\n";

            // Determine number of hardware threads
            unsigned int num_threads = std::thread::hardware_concurrency();
            if (num_threads == 0) num_threads = 4; // fallback

            vector<std::thread> threads;
            threads.reserve(num_threads);

            // Divide rows among threads
            for (unsigned int t = 0; t < num_threads; ++t) {
                threads.emplace_back([&, t]() {
                for (int j = t; j < height; j += num_threads) {
                    for (int i = 0; i < width; ++i) {
                        color pixel_color{0, 0, 0};
                        float s1 = 0.0f; // Sum of illuminance
                        float s2 = 0.0f; // Sum of illuminance^2
                        int sample_count = 0;
                        int min_samples = 64;
                        float tolerance = 0.02f;
                        float confidence = 1.96f;
                        for (int sample = 0; sample < samples_per_pixel; ++sample) {
                            ray r = get_ray(i, j);
                            color sample_color = ray_color(r, max_depth, world, color{1.0f, 1.0f, 1.0f});
                            pixel_color += sample_color;
                            // adaptive sampling
                            float illum  = 0.2126f * sample_color[0] + 0.7152f * sample_color[1] + 0.0722f * sample_color[2];
                            s1 += illum;
                            s2 += illum * illum;
                            sample_count++;
                            if (sample_count % 8 == 0 && sample_count >= min_samples){
                                float mean = s1 / sample_count;
                                float variance = (s2 - (s1 * s1) / sample_count) / (sample_count - 1);
                                float stddev = sqrt(variance);
                                float I = confidence * stddev / sqrt(sample_count); // 95% confidence interval

                                if (I <= tolerance * mean) { // Convergence tolerance
                                    break; // Pixel has converged!
                                }
                            }

                        }
                        framebuffer[j * width + i] = (float)pixel_samples_scale * pixel_color;
                    }
                }
                });
            }

            // Wait for all threads to finish
            for (auto& th : threads) th.join();

            // Output the framebuffer
            for (const color& c : framebuffer) {
                write_color(std::cout, c);
            }
            std::clog << "\rDone.                 \n";
        }

    private: 
        int height; // Rendered image height
        double pixel_samples_scale; // Color scale factor for a sum of pixel samples
        point3 center; // Camera center
        point3 pixel00_loc;  // Location of pixel 0, 0
        vec3 pixel_delta_u; // Offset to pixel to the right
        vec3 pixel_delta_v; // Offset to pixel below
        vec3 u, v, w; // Camera frame basis vectors
        vec3 defocus_disk_u; // Defocus disk horizonal radius
        vec3 defocus_disk_v; // Defocus disk vertical radius
        void initialize() {
            height = int(width / aspect_ratio);
            height = (height < 1) ? 1 : height;
            pixel_samples_scale = 1.0 / samples_per_pixel;
            center = lookfrom;

            // Determine viewport dimensions.
            //auto focal_length = (lookfrom - lookat).length();
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (double(width)/height);

            // Calc the u,v,w unit basis vectors for the cam's coord fram
            w = unit_vector(lookfrom-lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u =(float)viewport_width * u;
            vec3 viewport_v =  (float)viewport_height * -v;

            // Calculate the horizontal and vertical delta vectors from pixel to pixel.
            pixel_delta_u = viewport_u / (float)width;
            pixel_delta_v = viewport_v / (float)height;

            // Calculate the location of the upper left pixel.
            auto viewport_upper_left = center - ((float)focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            // Calc the camera defocus disk basis vectors.
            auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle/2));
            defocus_disk_u = u * (float)defocus_radius;
            defocus_disk_v = v * (float)defocus_radius;

        }

        ray get_ray(int i, int j) const {
            // Construct a camera ray originating from the origin and directed at randomly sampled
            // point around the pixel location i, j.

            auto offset = sample_square();
            auto pixel_sample = pixel00_loc
                            + ((i + offset[0]) * pixel_delta_u)
                            + ((j + offset[1]) * pixel_delta_v);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;

            return ray(ray_origin, ray_direction);
        }
    

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3{random_float() - 0.5f, random_float() - 0.5f, 0.0f};
    }

    point3 defocus_disk_sample() const {
            // random point in the camera defocus disk
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
        

        color ray_color(const ray& r, int depth, const hittable& world, const color& throughput) const{
            if (depth<= 0) return color{0,0,0}; // ray bounce limit
            hit_record rec; 
            if (world.hit(r, interval(0.001, infinity), rec)) {
                // legacy diffuse mat: vec3 direction = rec.normal + random_unit_vector();
                // const val is the reflectance. higher reflectance, the lighter the scene
                // return 0.1 * ray_color(ray(rec.p, direction), depth-1, world);

                ray scattered;
                color attenuation;

                // roulette check 
                if (rec.mat->scatter(r, rec, attenuation, scattered)){
                    color new_throughput = attenuation * throughput;
                    
                    float p = fmax(new_throughput[0], fmax(new_throughput[1], new_throughput[2]));
                    if (p < 1.0f && random_float() >p){
                        return color{0,0,0};
                    }
                    new_throughput = new_throughput * (1.0f/p);

                    return new_throughput * ray_color(scattered, depth-1, world, new_throughput);
                }
                return color{0,0,0};
            }
            vec3 unit_direction = unit_vector(r.direction());
            auto a = 0.5*(unit_direction[1] + 1.0);
            return (float)(1.0f-a)*color{1.0f, 1.0f, 1.0f} + (float)a*color{0.5f, 0.7f, 1.0f};
        }

};

#endif