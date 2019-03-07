#pragma once
#include <vector>
#include "Metric.h"
#include "Sphere.h"


struct Camera {
  Point3d pos, center;
  Vector3d dir, up, right, sx, sy;
  int xres, yres;
  int xsamples = 1;
  int ysamples = 1;
  double focalDist, aspect, verticleFov;
  Camera(const Point3d &pos, const Vector3d &dir, const Vector3d &up,
    const int xres, const int yres, const double focalDist, const double verticleFov) {
      this->pos = pos;
      this->dir = dir;
      this->right = right;
      this->up = up;
      this->xres = xres;
      this->yres = yres;
      this->focalDist = focalDist;
      this->aspect = (double)xres / yres;
      this->verticleFov = verticleFov;

      this->normalize();
  }

  void normalize() {
    this->dir = this->dir.normalize();
    this->right = this->dir.cross(this->up).normalize();
    this->up = this->right.cross(this->dir).normalize();

    this->center = this->pos + this->dir * focalDist;
    this->sy = this->up * tan(DEG_2_RAD(this->verticleFov) / 2.0) * focalDist;
    this->sx = this->right * tan(DEG_2_RAD(this->verticleFov) / 2.0) * focalDist * this->aspect;
  }

  Camera() {}
};

class Scene {
public:
  int n_objects = 0;
  std::vector<Object *> objects = std::vector<Object *>();
  Metric metric;
  Camera camera;
  void addObject(Object &obj);
  void setMetric(const Metric &m);
  double get_smallest_radius() const;
};
