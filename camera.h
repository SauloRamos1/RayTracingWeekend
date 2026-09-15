#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

#include "color.h"
#include "hittable.h"
#include "interval.h"
#include "material.h"
#include "ray.h"

class camera {
public:
    double aspect_ratio { 1.0 };
    int image_width { 100 };
    int samples_per_pixel { 10 };
    int max_depth { 50 };

    double vfov { 90 };
    point3 lookfrom {0, 0, 0};
    point3 lookat {0, 0, -1};
    vec3 vup { 0, 1, 0};

    double defocus_angle { 0 }; // Variation angle of rays through each pixel
    double focus_dist { 10 }; // Distance from camera lookfrom point to plane of perfect focus

    void render (const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << ' ' << image_height_ << "\n255\n";

        const int total_pixels { image_width * image_height_ };

        std::vector<color>image_buffer (total_pixels);
        std::vector<int>pixel_indices(total_pixels);
        std::iota(pixel_indices.begin(), pixel_indices.end(), 0);

        std::for_each (std::execution::par, pixel_indices.begin(), pixel_indices.end(), [&] (int index) {
            const int i = index % image_width;
            const int j = index / image_width;
            color pixel_color {0,0,0};
            for (int sample = 0; sample < samples_per_pixel; sample++) {
                ray r = get_ray(i,j);
                pixel_color += ray_color(r, max_depth, world);
            }
            image_buffer[index] = pixel_samples_scale_ * pixel_color;
        });

        for (int index = 0; index < total_pixels; index++) {
            write_color(std::cout, image_buffer[index]);
        }

        std::clog << "Done.\n";
    }

private:
    int image_height_ { 0 };   // Rendered image height
    double pixel_samples_scale_ {0};
    point3 center_;         // Camera center
    point3 pixel00_loc_;    // Location of pixel 0, 0
    vec3 pixel_delta_u_;  // Offset to pixel to the right
    vec3 pixel_delta_v_;  // Offset to pixel below
    vec3 u_, v_, w_;      // Camera frame basis vectors
    vec3 defocus_disk_u_; // Defocus disk horizontal radius
    vec3 defocus_disk_v_; // Defocus disk vertical radius

    void initialize() {
        image_height_ = static_cast<int>(image_width / aspect_ratio);
        image_height_ = image_height_ < 1 ? 1 : image_height_;

        pixel_samples_scale_ = 1.0 / samples_per_pixel;

        center_ = lookfrom;

        // Camera
        const auto theta = degrees_to_radians(vfov);
        const auto h = std::tan(theta / 2.0);
        const auto viewport_height = 2 * h * focus_dist;
        const auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height_);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w_ = unit_vector(lookfrom - lookat);
        u_ = unit_vector(cross(vup, w_));
        v_ = cross(w_, u_);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        const vec3 viewport_u = viewport_width * u_;    // Vector across viewport horizontal edge
        const vec3 viewport_v = viewport_height * -v_;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u_ = viewport_u / image_width;
        pixel_delta_v_ = viewport_v / image_height_;

        // Calculate the location of the upper left pixel.
        const auto viewport_upper_left = center_ - focus_dist * w_ - viewport_u/2 - viewport_v/2;
        pixel00_loc_ = viewport_upper_left + 0.5 * (pixel_delta_u_ + pixel_delta_v_);

        // Calculate the camera defocus disk basis vectors
        const auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle/2));
        defocus_disk_u_ = u_ * defocus_radius;
        defocus_disk_v_ = v_ * defocus_radius;
    }

    ray get_ray (const int i, const int j) const {
        // Camera ray originating from origin and directed at the randomly sampled point around pixel location (i,j).
        const auto offset = sample_square();
        const auto pixel_sample = pixel00_loc_ + (i+offset.x()) * pixel_delta_u_ + (j+offset.y()) * pixel_delta_v_;
        const auto ray_origin = (defocus_angle <= 0) ? center_ : defocus_disk_sample();
        const auto ray_direction = pixel_sample - ray_origin;
        return ray{ray_origin, ray_direction};
    }

    static vec3 sample_square () {
        return vec3 {random_double() - 0.5, random_double() - 0.5, 0};
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in rhe camera defocus disk
        auto p = random_in_unit_disk();
        return center_ + (p[0] * defocus_disk_u_) + (p[1] * defocus_disk_v_);
    }

    static color ray_color (const ray& r, const int depth, const hittable& world) {
        if (depth <=0)
            return color {0, 0, 0};
        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color {0, 0, 0};
        }

        const vec3 unit_direction = unit_vector(r.direction());
        const auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};

#endif // CAMERA_H