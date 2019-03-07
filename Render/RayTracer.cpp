#include <png++/png.hpp>
#include <cstdlib>
#include <thread>
#include "RayTracer.h"
#include "LightPath.h"

#define INTERIOR_STEP_FACTOR 1000.0

Color **RayTracer::pixel_buffer = NULL;
Scene *RayTracer::scene = NULL;
int RayTracer::xres = 0;
int RayTracer::yres = 0;
int RayTracer::diffuse_samples = 1;

void RayTracer::delete_buffer() {
  for (int i = 0; i < xres; i++) {
    delete[] pixel_buffer[i];
  }

  delete[] pixel_buffer;
  pixel_buffer = NULL;
}

void RayTracer::init_buffer() {
  assert(scene != NULL);
  if (pixel_buffer != NULL)
    delete_buffer();

  pixel_buffer = new Color *[xres];
  for (int i = 0; i < xres; i++) {
    pixel_buffer[i] = new Color[yres]();
  }
}

void RayTracer::load_scene(Scene &s) {
  scene = &s;
  if (scene->camera.xres != xres || scene->camera.yres != yres) {
    xres = scene->camera.xres;
    yres = scene->camera.yres;
    init_buffer();
  }
}

void RayTracer::write(const char *filename) {
  png::image<png::rgb_pixel> image(xres, yres);
  for (int j = 0; j < yres; j++) {
    for (int i = 0; i < xres; i++) {
      image.set_pixel(i, yres - j - 1, png::rgb_pixel(
        pixel_buffer[i][j][0],
        pixel_buffer[i][j][1],
        pixel_buffer[i][j][2]));
    }
  }

  image.write(filename);
}

void RayTracer::render(const double dt, const double soi_range, const int mode, const int n_threads) {
  assert(scene != NULL);

  std::thread *threads = new std::thread[n_threads];
  for (int i = 0; i < n_threads; i++) {
    int ymin, height;
    ymin = i * (yres / n_threads);
    if (i == n_threads - 1)
      height = yres - ymin;
    else
      height = yres / n_threads;

    threads[i] = std::thread(render_section, 0, ymin, xres, height, dt, soi_range, mode);
  }

  for (int i = 0; i < n_threads; i++) {
    threads[i].join();
  }

  delete[] threads;

}

void RayTracer::render_section(const int xmin, const int ymin, const int width, const int height,
  const double dt, const double soi_range, const int mode) {
  Color color;
  const int xsamples = scene->camera.xsamples;
  const int ysamples = scene->camera.ysamples;
  const int nsamples = xsamples * ysamples;
  const int xres = scene->camera.xres;
  const int yres = scene->camera.yres;
  Color total_color;

  for (int i = xmin; i < xmin + width; i++) {
    for (int j = ymin; j < ymin + height; j++) {
      total_color[0] = 0;
      total_color[1] = 0;
      total_color[2] = 0;
      //std::cout << i << ", " << j << std::endl;

      for (int x = 0; x < xsamples; x++) {
        for (int y = 0; y < ysamples; y++) {
          double dx = (double)i / xres;
          double dy = (double)j / yres;

          // jittering
          //dx += (((double)x + (double)rand() / RAND_MAX) / xsamples / xres);
          //dy += (((double)y + (double)rand() / RAND_MAX) / ysamples / yres);

          dx += (((double)x + 0.5) / xsamples / xres);
          dy += (((double)y + 0.5) / ysamples / yres);

          dx = 2.0 * dx - 1.0;
          dy = 2.0 * dy - 1.0;
          Point3d p = scene->camera.center + (scene->camera.sx * dx) + (scene->camera.sy * dy);
          Ray ray(scene->camera.pos, (p - scene->camera.pos).normalize());
          Sphere range = Sphere(scene->metric.origin, scene->metric.rs * soi_range);

          if (mode == EULER_RAY_TRACE) {
            euler_ray_trace(ray, dt, range, color);
          } else if (mode == ACCEL_RAY_TRACE || mode == ACCEL_PATH_TRACE) {
            accel_ray_trace(ray, dt, range, color, mode);
          } else {
            assert(false);
          }

          /*if (i == 630 || j == 700) {
            color[0] = 255.0f;
            color[1] = 0.0f;
            color[2] = 0.0f;
          }*/

          total_color[0] += color[0];
          total_color[1] += color[1];
          total_color[2] += color[2];
        }

      }

      //exit(0);

      pixel_buffer[i][j][0] = MIN(255, total_color[0] / nsamples);
      pixel_buffer[i][j][1] = MIN(255, total_color[1] / nsamples);
      pixel_buffer[i][j][2] = MIN(255, total_color[2] / nsamples);

    }
  }

}

void RayTracer::accel_ray_trace(Ray &ray, const double dt, const Sphere &range, Color color, const int mode) {
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  Material temp_material, material;
  Color temp_color;

  // if we are already in the sphere of influence we skip this part
  if (ray.O.dist(range.origin) <= range.radius + EPSILON)
    goto accel;

  double limit;
  if (!(range.intersects(ray, limit) && range.radius > 0.0)) {
    // if the ray does not intersect the sphere of influence we can trace it indefinitely
    if (trace_partial_ray(ray, 1e20, temp_color)) {
      memcpy(color, temp_color, sizeof(Color));
    }

    return;
  }

  // update the ray's origin to the limit
  ray.O = ray.O + ray.D * limit;
accel:
  double t, jump_dist, min_t;
  double dist_to_origin;
  bool intersection_found = false;
  Ray intersecting_ray, step_ray;
  Vector3d normal, temp_normal, step_dir;
  Object *closest;

  const double min_radius = scene->get_smallest_radius();

  while ((dist_to_origin = ray.O.dist(range.origin)) <= range.radius + EPSILON) {

    // Optimization for detecting light rays that cross event horizon
    if ((min_radius > scene->metric.rs * 3.0 / 2.0) && (dist_to_origin <= scene->metric.rs * 3.0 / 2.0)) {
      /* if there are no objects within the photon sphere, we can stop tracing
       a ray if it falls below the photon sphere */
       const Point3d O = ray.O - scene->metric.origin;

       /* if a ray has negative radial velocity at or below the photon sphere,
       it is guaranteed to fall through the event horizon */

       if (ray.D * O < 0.0) {
         return;
       }
    }


    jump_dist = 1e20;
    min_t = 1e20;
    closest = NULL;

    // Go through each of the objects and find the closest one
    for (std::vector<Object *>::const_iterator it = scene->objects.begin(); it != scene->objects.end(); it++) {
      if ((*it)->isBackground()) {
        continue;
      }

      const double d = (*it)->dist_from_point(ray.O);

      if (d < jump_dist) {
        jump_dist = d;
        closest = *it;
      }
    }

    // attempt to make a jump
    if (jump_dist > dt) {
      if (LightPath::ModelQuery(ray, jump_dist, scene->metric)) {
        continue;
      }
    }

    // if we weren't able to jump, step using Euler's method
    if (!scene->metric.step(dt, ray, step_ray)) {
      return;
    } else if (closest == NULL) {
      continue;
    }

    // check for an intersection

    if (closest->intersects(step_ray, dt, t, temp_material) && t < min_t) {
      min_t = t;
      material = temp_material;
      intersection_found = true;
      ray.O = step_ray.O + step_ray.D * t;
      ray.D = step_ray.D;

      // edge case: if the intersection occurs below the event horizon it doesnt count
      if (ray.O.dist(scene->metric.origin) <= scene->metric.rs) {
        return;
      }

      break;
    }

  }

  // if the ray never intersected an object or the event horizon
  // we need to trace it outside of the sphere of influence
  if (intersection_found == false) {
    if (trace_partial_ray(ray, 1e20, temp_color)) {
      memcpy(color, temp_color, sizeof(Color));
    }

    return;
  }

  // we deal with the intersection
  color[0] = material.color[0] * material.ambient * material.opacity;
  color[1] = material.color[1] * material.ambient * material.opacity;
  color[2] = material.color[2] * material.ambient * material.opacity;

  if (mode == ACCEL_RAY_TRACE) {
    const float diff = MAX(0.0f, material.diffuse * -(material.normal * material.lighting));

    color[0] += material.color[0] * diff * material.opacity;
    color[1] += material.color[1] * diff * material.opacity;
    color[2] += material.color[2] * diff * material.opacity;
  } else if (mode == ACCEL_PATH_TRACE && material.diffuse > 0.0f) {
    Ray reflected;
    Color diff = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i < diffuse_samples; i++) {
      reflected.D = Vector3d::rand_weighted_unit_vec_in_hemisphere(material.normal);
      reflected.O = ray.O + reflected.D * EPSILON;

      accel_ray_trace(reflected, dt, range, temp_color, mode);
      //const double cos_theta = reflected.D * material.normal;
      diff[0] += (temp_color[0] / 255.0f) * material.color[0] * material.diffuse * material.opacity * M_PI;
      diff[1] += (temp_color[1] / 255.0f) * material.color[1] * material.diffuse * material.opacity * M_PI;
      diff[2] += (temp_color[2] / 255.0f) * material.color[2] * material.diffuse * material.opacity * M_PI;
    }

    color[0] += diff[0] / diffuse_samples;
    color[1] += diff[1] / diffuse_samples;
    color[2] += diff[2] / diffuse_samples;
  }


  if (material.specular > 0.0f) {
    Ray reflected;
    reflected.D = ray.D - material.normal * 2.0 * (ray.D * material.normal);
    reflected.O = ray.O + reflected.D * EPSILON;

    accel_ray_trace(reflected, dt, range, temp_color, mode);

    color[0] += temp_color[0] * material.specular;
    color[1] += temp_color[1] * material.specular;
    color[2] += temp_color[2] * material.specular;

  }

  if (material.opacity >= 1.0f)
    return;

  Ray newRay;
  ray.O = ray.O + ray.D * EPSILON;

  accel_ray_trace(ray, dt, range, temp_color, mode);
  color[0] += temp_color[0] * (1.0 - material.opacity);
  color[1] += temp_color[1] * (1.0 - material.opacity);
  color[2] += temp_color[2] * (1.0 - material.opacity);

}

void RayTracer::euler_ray_trace(Ray &ray, const double dt, const Sphere &range, Color color) {
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  double t;
  Color temp_color;
  Material material;

  // if we are already in the sphere of influence we skip this part
  if (ray.O.dist(range.origin) <= range.radius + EPSILON)
    goto euler;

  // determine how far we can safely trace the ray in a straight line
  double limit;
  if (!(range.intersects(ray, limit) && range.radius > 0.0)) {
    // if the ray does not intersect the sphere of influence we can trace it indefinitely
    if (trace_partial_ray(ray, 1e20, temp_color))
      memcpy(color, temp_color, sizeof(Color));

    return;
  }

  // update the ray's origin to the limit
  ray.O = ray.O + ray.D * limit;

  // perform euler's method to trace the curved ray
euler:
  Ray step_ray;
  while (ray.O.dist(range.origin) <= range.radius + EPSILON) {

    const double actual_dt = scene->metric.get_step_size(dt, ray);

    if (!scene->metric.step(actual_dt, ray, step_ray)) {
      return;
    }

    for (std::vector<Object *>::iterator it = scene->objects.begin(); it != scene->objects.end(); it++) {
      if ((*it)->isBackground())
        continue;

      if ((*it)->intersects(step_ray, actual_dt, t, material)) {
        const float diff = MAX(0.0f, material.diffuse * -(material.normal * material.lighting));

        color[0] = material.color[0] * (material.ambient + diff) * material.opacity;
        color[1] = material.color[1] * (material.ambient + diff) * material.opacity;
        color[2] = material.color[2] * (material.ambient + diff) * material.opacity;

        if (material.specular > 0.0f) {
          Ray reflected;
          reflected.O = step_ray.O + step_ray.D * t;
          reflected.D = step_ray.D - material.normal * 2.0 * (step_ray.D * material.normal);
          reflected.O = reflected.O + reflected.D * EPSILON;

          euler_ray_trace(reflected, dt, range, temp_color);

          color[0] += temp_color[0] * material.specular;
          color[1] += temp_color[1] * material.specular;
          color[2] += temp_color[2] * material.specular;

        }

        if (material.opacity >= 1.0)
          return;

        Ray newRay = Ray(step_ray.O + step_ray.D * (t + EPSILON), step_ray.D);
        euler_ray_trace(newRay, dt, range, temp_color);

        color[0] += temp_color[0] * (1.0 - material.opacity);
        color[1] += temp_color[1] * (1.0 - material.opacity);
        color[2] += temp_color[2] * (1.0 - material.opacity);

        return;
      }

    }


  }

  // we have left the sphere of influence and are done with euler's method
  // finally we trace the ray out to infinity
  if (trace_partial_ray(ray, 1e20, temp_color)) {
    memcpy(color, temp_color, sizeof(Color));
  }

}

// Traces a ray up to a maximum distance of "limit"
bool RayTracer::trace_partial_ray(const Ray &ray, const double limit, Color color) {
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  double t;
  double max_t = 1e20;
  Color temp_color;
  Material temp_material, material;
  bool intersection_found = false;

  for (std::vector<Object *>::iterator it = scene->objects.begin(); it != scene->objects.end(); it++) {
    if ((*it)->intersects(ray, MIN(limit, max_t), t, temp_material)) {
      material = temp_material;
      max_t = t;
      intersection_found = true;
    }
  }

  if (intersection_found == false)
    return false;

  const float diff = MAX(0.0f, material.diffuse * -(material.normal * material.lighting));

  color[0] = material.color[0] * (material.ambient + diff) * material.opacity;
  color[1] = material.color[1] * (material.ambient + diff) * material.opacity;
  color[2] = material.color[2] * (material.ambient + diff) * material.opacity;

  if (material.specular > 0.0f) {
    Ray reflected;
    reflected.O = ray.O + ray.D * max_t;
    reflected.D = ray.D - material.normal * 2.0 * (ray.D * material.normal);
    reflected.O = reflected.O + reflected.D * EPSILON;
    if (trace_partial_ray(reflected, limit - max_t - EPSILON, temp_color)) {
      color[0] += temp_color[0] * material.specular;
      color[1] += temp_color[1] * material.specular;
      color[2] += temp_color[2] * material.specular;
    }
  }

  if (material.opacity >= 1.0f)
    return true;

  Ray newRay = Ray(ray.O + ray.D * (max_t + EPSILON), ray.D);
  if (trace_partial_ray(newRay, limit - max_t - EPSILON, temp_color)) {

    color[0] += temp_color[0] * (1.0 - material.opacity);
    color[1] += temp_color[1] * (1.0 - material.opacity);
    color[2] += temp_color[2] * (1.0 - material.opacity);
  }


  return true;
}
