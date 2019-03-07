#pragma once
#include "Object.h"

class Sphere : public Object {
protected:
  Vector3d up = UP;
  Vector3d right = RIGHT;
  Vector3d in = UP.cross(RIGHT);
public:
  Point3d origin;
  double radius;

  Sphere(const Point3d &origin, const double radius);
  Sphere(const Point3d &origin, const double radius, const Color color);
  Sphere(const Point3d &origin, const double radius, const char *filename);

  double dist_from_point(const Point3d &p) const;
  bool intersects(const Ray &ray, const double max_t, double &t, Material &m) const;
  bool intersects(const Ray &ray, double &t) const;
  void setAxis(const Vector3d &up, const Vector3d &right);
};

class Background : public Sphere {
public:
  bool isBackground() const { return true; }
  //double dist_from_point(const Point3d &p) const { return 1e20; }
  Background(const Point3d &origin, const double radius, const char *filename)
            : Sphere::Sphere(origin, radius, filename) {};


};
