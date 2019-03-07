#pragma once
#include "Vector.h"

class Metric {
public:
  double rs;
  Vector3d origin;
  Metric(const double rs, const Vector3d &origin);
  Metric();
  double get_step_size(const double dt, const Ray &ray) const;
  bool step(const double dt, Ray &ray, Ray &step_ray) const;
  bool step(const double dt, Ray &ray) const;
  void timelike_step(const double time, const double dt, Ray &ray) const;
};
