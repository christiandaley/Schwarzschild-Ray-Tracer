#include <cstring>
#include <png++/png.hpp>
#include "Sphere.h"

Sphere::Sphere(const Point3d &origin, const double radius, const Color color) {
  this->origin = origin;
  this->radius = radius;
  memcpy(this->material.color, color, sizeof(Color));
}

Sphere::Sphere(const Point3d &origin, const double radius) {
  this->origin = origin;
  this->radius = radius;
}

Sphere::Sphere(const Point3d &origin, const double radius, const char *filename) {
  this->origin = origin;
  this->radius = radius;
  this->load_tx(filename);
}

double Sphere::dist_from_point(const Point3d &p) const {
  const double d = this->origin.dist(p) - this->radius;
  //return d;
  return d >= 0.0 ? d : -d;
}

bool Sphere::intersects(const Ray &ray, const double max_t, double &t, Material &m) const {
  const Point3d O = ray.O - this->origin;
  const Vector3d &D = ray.D;

  // (O + Dt)(O + Dt) = R^2
  // (D*D)t^2 + 2ODt + O^2 - R^2 = 0
  const double a = D * D;
  const double b = 2.0 * (O * D);
  const double c = O * O - this->radius * this->radius;
  const double disc = b * b - 4.0 * a * c;
  if (disc < 0.0)
    return false;

  const double disc_root = sqrt(disc);
  t = 0.0;

  const double t1 = (-b + disc_root) / 2.0 / a;
  const double t2 = (-b - disc_root) / 2.0 / a;
  if (t1 > 0.0)
    t = t1;
  if (t2 > 0.0 && (t2 < t1 || t1 <= 0.0))
    t = t2;

  if (t <= 0.0 || t > max_t)
    return false;

  const Point3d P = (O + D * t);
  m = this->material;
  m.normal = P.normalize();

  if (hasTexture) {
    const double px = P * this->right;
    const double py = P * this->up;
    const double pz = P * this->in;


    double phi = atan2(py, px);
    phi += phi >= 0.0 ? 0.0 : 2.0 * M_PI;

    double theta = acos(MIN(1.0, pz / this->radius));

    double x = (phi / 2.0 / M_PI) * (tx_width - 2);
    double y = (theta / M_PI) * (tx_height - 2);
    assert(y >= 0 && y < tx_height);
    assert(x >= 0 && x < tx_width);

    this->tx_lookup(x, y, m.color);

  }

  return true;
}

bool Sphere::intersects(const Ray &ray, double &t) const {
  const Point3d O = ray.O - this->origin;
  const Vector3d &D = ray.D;

  // (O + Dt)(O + Dt) = R^2
  // (D*D)t^2 + 2ODt + O^2 - R^2 = 0
  const double a = D * D;
  const double b = 2.0 * (O * D);
  const double c = O * O - this->radius * this->radius;
  const double disc = b * b - 4.0 * a * c;
  if (disc < 0.0)
    return false;

  const double disc_root = sqrt(disc);
  t = 0.0;

  const double t1 = (-b + disc_root) / 2.0 / a;
  const double t2 = (-b - disc_root) / 2.0 / a;
  if (t1 > 0.0)
    t = t1;
  if (t2 > 0.0 && (t2 < t1 || t1 <= 0.0))
    t = t2;

  return t > 0.0;
}

void Sphere::setAxis(const Vector3d &up, const Vector3d &right) {
  this->up = up.normalize();
  this->right = (right - this->up * (this->up * right)).normalize();
  this->in = up.cross(this->right).normalize();
}
