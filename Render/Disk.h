#pragma once

#include "Object.h"

class Disk : public Object {
public:
  Point3d origin;
  Vector3d up, right, in;
  double inner_radius, outer_radius;
  double theta_offset = 0.0;

  Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
      const double inner_radius, const double outer_radius);

  Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
      const double inner_radius, const double outer_radius, Color color);

  Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
      const double inner_radius, const double outer_radius, const char *filename);

  double dist_from_point(const Point3d &p) const;
  virtual bool intersects(const Ray &ray, const double max_t, double &t, Material &m) const;
};
