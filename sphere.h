#ifndef SPHERE_H
#define SPHERE_H

#include <utility>

#include "hittable.h"

class sphere final : public hittable {

public:
    sphere (const point3&  center, const double radius, shared_ptr<material> mat)
    : center_(center), radius_(std::fmax(0,radius)), mat_(std::move(mat)) {
        // TODO: Initialize the material pointer `mat`
    }

    bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override {
        const vec3 oc = center_ - r.origin();
        const auto a = r.direction().length_squared();
        const auto h = dot (r.direction(), oc);
        const auto c = oc.length_squared() - radius_ * radius_;

        const auto discriminant = h*h - a*c;
        if (discriminant < 0) {
            return false;
        }
        const auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = r.at (rec.t);
        const vec3 outward_normal = (rec.p - center_) / radius_;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat_;

        return true;
    }
private:
    point3 center_;
    double radius_;
    shared_ptr<material> mat_;
};

#endif //SPHERE_H
