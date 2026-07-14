#ifndef VEC3_H
#define VEC3_H
#include <simd/simd.h>
#include "main_header.h"
#include <ostream>
using namespace std;

// class vec3{
//     public:
//         double e[3];
//         vec3() : e{0,0,0} {}
//         vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

//         double x() const {return e[0]; }
//         double y() const {return e[1]; }
//         double z() const {return e[2]; }

//         double operator[](int i) const { return e[i]; }
//         double& operator[](int i) { return e[i]; }

//         vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
//         vec3& operator+=(const vec3& v) {
//             e[0] += v.e[0];
//             e[1] += v.e[1];
//             e[2] += v.e[2];
//             return *this;
//         }

//         vec3& operator*=(double t) {
//             e[0] *= t;
//             e[1] *= t;
//             e[2] *= t;
//             return *this;
//         }

//         vec3& operator/=(double t) {
//             return *this *= 1/t;
//         }

//         double length() const {
//             return sqrt(length_squared());
//         }

//         double length_squared() const {
//             return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
//         }

//         bool near_zero() const{
//             // true if vector is close to zero in all dims
//             auto s = 1e-8;
//             return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
//         }

//         static vec3 random() {
//             return vec3(random_double(), random_double(), random_double());
//         }

//         static vec3 random(double min, double max) {
//             return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
//         }
// };


using point3 = simd_float3;
using vec3 = simd_float3;


inline ostream& operator << (ostream& out, const vec3& v){
    return out << v[0] << v[1] << v[2];
}

// inline vec3 operator+(const vec3& u, const vec3& v){
//     return simd_add(u, v);
// }

// inline vec3 operator-(const vec3& u, const vec3& v){
//     return simd_sub(u, v)
// }

// inline vec3 operator*(const vec3& u, const vec3& v){
//     return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
// }
// inline vec3 operator*(const vec3& v, double t){
//     return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
// }
// inline vec3 operator*(double t, const vec3& v){
//     return v * t;
// }

// inline vec3 operator/(const vec3& v, double t){
//     return (1/t) * v;
// }

// inline updates
inline float length(const vec3& v) {
    return simd_length(v);
}

inline vec3 unit_vector(const vec3& v) {
    return simd_normalize(v);
}

inline float length_squared(const vec3& v) {
    return simd_length_squared(v);
}

inline bool near_zero(const vec3& v) {
        // true if vector is close to zero in all dims
        //auto s = 1e-8;
        //return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
        return length_squared(v) < 1e-8f;
}

inline float random_float(){
    return static_cast<float>(random_double());
}
inline float random_float(float min, float max){
    return min + (max - min) * random_float();
}


inline vec3 random_vec(float min, float max) {
    return vec3{random_float(min, max), random_float(min, max), random_float(min, max)};
}

inline vec3 random_vec() {
    return vec3{random_float(), random_float(), random_float()};
}

inline float dot(const vec3& u, const vec3& v) {
    return simd_dot(u, v);
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return simd_cross(u, v);
}




inline vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3{random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), 0.0f};
        if (length_squared(p) < 1.0f) return p;
    }
}

inline vec3 random_unit_vector() {
    while (true){
        vec3 p = random_vec(-1.0f, 1.0f);
        float lensq = length_squared(p);
        if (1e-8f < lensq && lensq<=1.0f){
            return unit_vector(p);
        }

    }
}

inline vec3 random_on_hemisphere(const vec3& normal){
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0f){ // in same hemisphere as the normal?
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline vec3 reflect(const vec3& v, const vec3& n){
    return v - 2*dot(v, n) * n;
}

// snell's law
inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat){
    float cos_theta = fminf(dot(-uv, n), 1.0f);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    float r2 = length_squared(r_out_perp);
    vec3 r_out_parallel = -sqrtf(fabsf(1.0f - r2)) * n;
    return r_out_perp + r_out_parallel;
}




// inline vec3 random(double min, double max) {
//     return vec3{random_double(min, max), random_double(min, max), random_double(min, max)};
// }





#endif