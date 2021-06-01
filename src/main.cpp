// Utility Functions
#include "main.h"

// Classes
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "aarect.h"

// Multithreading Support
#include "external/ThreadPool.h"

// System Header Files
#include <fstream>

// Determining the color hit by ray
color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we have exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return color(0,0,0);

    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

/* SCENES */

// Cornell Box
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    return objects;
}

// Simple Light
hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

// Earth Sphere
hittable_list earth() {
    // By default stb_image library will look for the image file in the same directory as the executable,
    // So either copy the 'earthmap.jpg' in the same directory as the executable i.e. "build/" on Linux or "build/Release" on Windows,
    // Or give the absolute path to the image file which is found in the base directory of the repository.
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}


// Two Perlin Spheres
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);

    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0,  2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

// Two Checkered Spheres
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

// Randomly Generated Scene
hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    // Randomly Generated Small Spheres
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
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

    // Three Big Spheres
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

/* MULTITHREADING */

std::mutex cnt_mutex;
int rendered_pixels = 0;

// Calculating color value for each pixel
void render_pixel(int j, int i, const color& background, const hittable_list& world, const camera& cam, int w, int h,
                  int samples_per_pixel, int max_depth, std::vector<color>& result) {
    color pixel_color(0, 0, 0);
    for (int s = 0; s < samples_per_pixel; ++s) {
        auto u = (i + random_double()) / (w - 1);
        auto v = (j + random_double()) / (h - 1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, background, world, max_depth);
    }
    cnt_mutex.lock();
    rendered_pixels++;
    std::cerr << "\r" << (rendered_pixels * 100) / (w * h) << "%" << ' ' << std::flush;   // Rendering Progress
    cnt_mutex.unlock();
    result[j * w + i] = pixel_color;
}

// Rendering the image concurrently using threads
void concurrent_render(const int thread_cnt, const color& background, const hittable_list& world, const camera& cam, int image_width,
                       int image_height, int samples_per_pixel, int max_depth, std::ofstream& outputStream) {
    progschj::ThreadPool thread_pool(thread_cnt);
    std::vector<color> result(image_width * image_height);
    std::cerr << ">>> RENDERING <<<" << std::endl;
    // Rendering the image
    for (int j = image_height-1; j >= 0; --j) {

        for (int i = 0; i < image_width; ++i) {
            thread_pool.enqueue([j, i, &background, &world, cam, image_width, image_height, samples_per_pixel, max_depth, &result] () {
                render_pixel(j, i, background, world, cam, image_width, image_height, samples_per_pixel, max_depth, result);
            });
        }
    }
    thread_pool.wait_until_nothing_in_flight();
    std::cerr << "\r";
    std::cerr << ">>> WRITING TO FILE <<<" << std::endl;
    // Writing the image to file
    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            write_color(outputStream, result[j * image_width + i], samples_per_pixel);
        }
    }
}

// Main Function
int main() {

    // Setting the image dimensions
    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 100;
    const int max_depth = 50;

    // Setting the world
    hittable_list world;

    // Setting the camera
    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0, 0, 0);

    switch (0) {
        case 1:
            world = random_scene();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        case 2:
            world = two_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;

        case 3:
            world = two_perlin_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2 ,3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;

        case 4:
            world = earth();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13, 2, 3);
            lookat = point3(0, 0, 0);
            vfov = 20.0;
            break;

        case 5:
            world = simple_light();
            samples_per_pixel = 400;
            background = color(0, 0, 0);
            lookfrom = point3(26, 3, 6);
            lookat = point3(0, 2, 0);
            vfov = 20.0;
            break;

        default:
        case 6:
            world = cornell_box();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            background = color(0, 0, 0);
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;
    }

    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // Preparation for output image file (using .ppm format)
    std::ofstream outputStream ("output.ppm");   // Setting a file pointer and opening the file output.ppm
    outputStream << "P3\n" << image_width << ' ' << image_height << "\n255\n";       // Writing .ppm header

    // Initialize Multithreading
    int thread_cnt;
    std::cout << "How many threads you want to use for rendering? (Default 4 if no input given) : ";
    if (std::cin.peek() == '\n') {
        thread_cnt = 4;
    } else {
        std::cin >> thread_cnt;
    }
    concurrent_render(thread_cnt, background, world, cam, image_width, image_height, samples_per_pixel, max_depth, outputStream);

    // Printing completion message
    std::cerr << "\nDone. A new output.ppm file has been generated.\n";
}
