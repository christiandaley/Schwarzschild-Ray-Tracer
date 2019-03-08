#pragma once
#include "Scene.h"

#define EULER_RAY_TRACE 0
#define EULER_PATH_TRACE 1
#define ACCEL_RAY_TRACE 2
#define ACCEL_PATH_TRACE 3

class RayTracer {
  static Scene *scene;
  static int xres;
  static int yres;
  //static unsigned int ***pixel_buffer;
  //static Scene *scene;
  static void render_section(const int xmin, const int ymin, const int width, const int height,
                            const double dt, const double soi_range, const int mode);
  static void accel_ray_trace(Ray &ray, const double dt, const Sphere &range, Color color, const int mode);
  static void euler_ray_trace(Ray &ray, const double dt, const Sphere &range, Color color, const int mode);
  static bool trace_partial_ray(const Ray &ray, const double limit, Color color, const int mode);

  static void delete_buffer();
  static void init_buffer();
public:
  static int diffuse_samples;
  static Color **pixel_buffer;
  static void load_scene(Scene &scene);
  static void render(const double dt, const double soi_range, const int mode, const int n_threads);
  static void write(const char *filename);
};
