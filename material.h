#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class material {
    public:
    virtual ~material() = default;
    virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian final : public material {

public:
    explicit lambertian (const color& albedo) : albedo_ (albedo) {}

    bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const override {

        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray (rec.p, scatter_direction);
        attenuation = albedo_;
        return true;
    }

private:
    color albedo_;
};

class metal final : public material {
    public:
    explicit metal (const color& albedo, const double& fuzz) : albedo_ (albedo), fuzz_ (fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector (reflected) + fuzz_ * random_unit_vector();
        scattered = ray (rec.p, reflected);
        attenuation = albedo_;
        return dot(scattered.direction(), rec.normal) > 0;
    }

private:
    color albedo_;
    double fuzz_;
};

class dielectric final : public material {
public:
    explicit dielectric(const double& refraction_index) : refraction_index_ (refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        const double ri = rec.front_face ? (1.0/refraction_index_) : refraction_index_;
        const vec3 unit_direction = unit_vector (r_in.direction());
        const double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        const bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

       if (cannot_refract || reflectance (cos_theta, ri) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, ri);
        }
        scattered = ray(rec.p, direction);
        return true;
    }

private:
    double refraction_index_;

    static double reflectance (double cosine, double refraction_index) {
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 * (1-r0)*std::pow((1 - cosine), 5);
    }
};

#endif //MATERIAL_H
