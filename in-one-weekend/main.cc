#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

// Reference: https://raytracing.github.io/books/RayTracingInOneWeekend.html#overview

// Returns the color for a given scene ray
color ray_color(const ray& r, const hittable& world) {
    hit_record rec;

    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    // Figuring out the ray direction
    vec3 unit_direction = unit_vector(r.direction());

    // This alpha goes from 0 to one, since y goes from -1 to 1 after linearizing
    auto a = 0.5 * (unit_direction.y() + 1.0);

    // Returning a linear gradient based on alpha (therefore y)
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main() {
    // Image
    // Note that this is an ideal ratio, not the actual one that will be used,
    // since we're working with integers
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;

    // Calculate the image height, and ensure that it's at least 1
    int image_height = int(image_width / aspect_ratio);
    image_height = (image_height > 1) ? image_height : 1;

    // World
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera

    // This is the distance from the "eye" (camera POV) to the viewport
    auto focal_length = 1.0;
    auto camera_center = point3(0, 0, 0);

    // The viewport is a virtual rectangle in the 3D world that contains the grid of image pixel locations
    // Viewport widths less than one are ok since they are real valued
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges,
    // note that the vertical vector is in the opposite direction since we generate the image top-to-bottom
    // but for our camera the Y axis goes up
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // This makes sense because the camera center is in the middle of the viewport (0, 0, 0), so we remove
    // the focal distance to the viewport and then move it half the width left and up
    auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

    // Calculate the location of the upper left pixel (not on the very edge)
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "Scanlines remaining: " << (image_height - j) << '\n' << std::flush;

        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.\n";
}
