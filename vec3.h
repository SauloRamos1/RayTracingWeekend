#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class vec3 {
public:
    double e[3];

    vec3() : e {0, 0, 0} {}
    vec3(const double x, const double y, const double z) : e {x, y, z} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    // operator<any> defines what <any> do
    vec3 operator-() const { return vec3{-e[0], -e[1], -e[2]}; }
    double operator[](const int i) const {return e[i];}
    double& operator[](const int i) {return e[i];} // double& -> returns a reference

    vec3& operator+= (const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*= (const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/= (const double t) {
        return *this *= 1 / t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() const {
        constexpr auto s = 1e-8;
        return std::fabs(e[0]) < s && std::fabs(e[1]) < s && std::fabs(e[2]) < s;
    }

    static vec3 random() {
        return vec3{random_double(), random_double(), random_double()};
    }

    static vec3 random(const double& min, const double& max) {
        return vec3 {random_double(min, max), random_double(min, max), random_double(min, max)};
    }
};

using point3 = vec3; // point3 is an alias for vec3, useful for clarity in code

// Vector Utility Functions
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3{u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]};
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3{u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]};
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3{u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]};
}

inline vec3 operator*(const vec3& u, const double t) {
    return vec3{u.e[0] * t, u.e[1] * t, u.e[2] * t};
}

inline vec3 operator*(const double t, const vec3& u) {
    return vec3{u.e[0] * t, u.e[1] * t, u.e[2] * t};
}

inline vec3 operator/(const vec3& u, const double t) {
    return vec3{u.e[0] / t, u.e[1] / t, u.e[2] / t};
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3{
        u.e[1] * v.e[2] - u.e[2] * v.e[1],
        u.e[2] * v.e[0] - u.e[0] * v.e[2],
        u.e[0] * v.e[1] - u.e[1] * v.e[0]
    };
}

inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3{random_double(-1, 1), random_double(-1, 1), 0};
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    while (true) {
        const auto p = vec3::random(-1, 1);
        const auto lensq = p.length_squared();
        if (1e-160 < lensq && lensq <= 1)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    const vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;
    return -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3& uv, const vec3& n, const double& etai_over_etat) {
    const auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    const auto r_out_perp = etai_over_etat * (uv + cos_theta*n);
    const auto r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif //VEC3_H