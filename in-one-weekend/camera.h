#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera {
    public:
        // Ratio of image width over height
        // Note that this is an ideal ratio, not the actual one that will be used,
        // since we're working with integers
        double aspect_ratio = 16.0 / 9.0;

        // Rendered image width in pixel count
        int image_width = 1200;

        void render(const hittable& world) {
            initialize();

            std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

            for (int j = 0; j < image_height; j++) {
                std::clog << "Scanlines remaining: " << (image_height - j) << '\n' << std::flush;

                for (int i = 0; i < image_width; i++) {
                    auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                    auto ray_direction = pixel_center - center;
                    ray r(center, ray_direction);

                    color pixel_color = ray_color(r, world);
                    write_color(std::cout, pixel_color);
                }
            }

            std::clog << "\rDone.\n";

        }
    private:
        // Rendered image height in pixels
        int image_height;

        // Camera center
        point3 center;

        // Location of pixel 0, 0
        point3 pixel00_loc;

        // Offset to pixel to the right
        vec3 pixel_delta_u;

        // Offset to pixel below
        vec3 pixel_delta_v;

        void initialize() {
            // Calculate the image height, and ensure that it's at least 1
            image_height = int(image_width / aspect_ratio);
            image_height = (image_height > 1) ? image_height : 1;

            center = point3(0, 0, 0);

            // This is the distance from the "eye" (camera POV) to the viewport
            auto focal_length = 1.0;

            // Determine viewport dimensions
            // The viewport is a virtual rectangle in the 3D world that contains
            // the grid of image pixel locations
            // Viewport widths less than one are ok since they are real valued
            auto viewport_height = 2.0;
            auto viewport_width = viewport_height * (double(image_width) / image_height);

            // Calculate the vectors across the horizontal and down the vertical viewport edges,
            // note that the vertical vector is in the opposite direction since we
            // generate the image top-to-bottom but for our camera the Y axis goes up
            auto viewport_u = vec3(viewport_width, 0, 0);
            auto viewport_v = vec3(0, -viewport_height, 0);

            // Calculate the horizontal and vertical delta vectors from pixel to pixel
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            // This makes sense because the camera center is in the middle of the viewport (0, 0, 0),
            // so we remove the focal distance to the viewport and then move it half the width left and up
            auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;

            // Calculate the location of the upper left pixel (not on the very edge)
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        }

        // Returns the color for a given scene ray
        color ray_color(const ray& r, const hittable& world) const {
            hit_record rec;

            if (world.hit(r, interval(0, infinity), rec)) {
                return 0.5 * (rec.normal + color(1, 1, 1));
            }

            // Figuring out the ray direction
            vec3 unit_direction = unit_vector(r.direction());

            // This alpha goes from 0 to one, since y goes from -1 to 1 after linearizing
            auto a = 0.5 * (unit_direction.y() + 1.0);

            // Returning a linear gradient based on alpha (therefore y)
            return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
        }
};

#endif
