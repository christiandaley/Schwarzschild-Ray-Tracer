#include <cstring>
#include <assert.h>
#include "Disk.h"

Disk::Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
    const double inner_radius, const double outer_radius) {

    this->origin = origin;
    this->up = up.normalize();
    this->right = (right - up * (up * right)).normalize();
    this->in = this->up.cross(this->right).normalize();

    this->inner_radius = inner_radius;
    this->outer_radius = outer_radius;

}

Disk::Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
    const double inner_radius, const double outer_radius, Color color) {

    this->origin = origin;
    this->up = up.normalize();
    this->right = (right - up * (up * right)).normalize();
    this->in = this->up.cross(this->right).normalize();

    this->inner_radius = inner_radius;
    this->outer_radius = outer_radius;
    memcpy(this->material.color, color, sizeof(Color));
}


Disk::Disk(const Point3d &origin, const Vector3d &up, const Vector3d &right,
    const double inner_radius, const double outer_radius, const char *filename) {

    this->origin = origin;
    this->up = up.normalize();
    this->right = (right - up * (up * right)).normalize();
    this->in = this->up.cross(this->right).normalize();

    this->inner_radius = inner_radius;
    this->outer_radius = outer_radius;
    this->load_tx(filename);
}

double Disk::dist_from_point(const Point3d &p) const {
  const Vector3d V = p - this->origin;
  const double y = (V * this->up);
  const double x = (V * this->right);
  const double z = (V * this->in);

  // we project the point onto the plane containing the disk
  // then we consider three possible cases


  const double radial_dist = x * x + z * z;

  // the point lies between the inner and outer radius
  if ((radial_dist >= this->inner_radius * this->inner_radius)
    && (radial_dist <= this->outer_radius * this->outer_radius)) {
    return y >= 0 ? y : -y;
  }

  // the point lies within the inner radius
  if (radial_dist < this->inner_radius * this->inner_radius) {
    const double diff = this->inner_radius - sqrt(x * x + z * z);
    return sqrt(diff * diff + y * y);
  }

  // the point lies outside the outer radius
  if (radial_dist > this->outer_radius * this->outer_radius) {
    const double diff = sqrt(x * x + z * z) - this->outer_radius;
    return sqrt(diff * diff + y * y);
  }

  assert(false);
  return 0.0;
}

bool Disk::intersects(const Ray &ray, const double max_t, double &t, Material &m) const {
  Vector3d O = ray.O - this->origin;
  Vector3d D = ray.D;
  Point3d P;
  t = 0.0;
  // edge case: the ray is parallel to the disk plane

  if (D * this->up == 0.0) {
    // if the ray does not lie within the plane, return false
    if (O * this->up != 0.0) {
      return false;
    }


    // determine if the ray intersects the disk
    double a, b, c, disc, disc_root;
    a = D * D;
    b = 2.0 * (O * D);

    if (O.dist(this->origin) >= this->outer_radius) {
      c = O * O - (this->outer_radius * this->outer_radius);
    } else if (O.dist(this->origin) <= this->inner_radius) {
      c = O * O - (this->inner_radius * this->inner_radius);
    } else {
      P = O;
      goto texture;
    }

    disc = b * b - 4.0 * a * c;
    if (disc < 0.0)
      return false;

    disc_root = sqrt(disc);

    const double t1 = (-b + disc_root) / 2.0 / a;
    const double t2 = (-b - disc_root) / 2.0 / a;
    if (t1 > 0.0)
      t = t1;
    if (t2 > 0.0 && (t2 < t1 || t1 <= 0.0))
      t = t2;

    if (t <= 0.0 || t > max_t)
      return false;

    P = O + D * t;
  } else {

    // (O + Dt) * up = 0
    // (O * up) + (D * up)t = 0
    // t = -(O * up) / (D * up)

    // determine where the ray intersects the plane containing the disk

    t = -(O * this->up) / (D * this->up);

    if (t <= 0.0 || t > max_t)
      return false;

    // now determine if that intersection lies within the disk
    P = O + D * t;
    if ((P * P < this->inner_radius * this->inner_radius) || (P * P > this->outer_radius * this->outer_radius)) {
      return false;
    }
  }

texture:
  const double r = P.mag();

  m = this->material;
  m.normal = (P * this->up >= 0.0) ? this->up : -this->up;

  if (this->hasTexture) {
    double theta = atan2(P * this->in, P * this->right);
    theta += theta >= 0.0 ? 0.0 : 2.0 * M_PI;

    theta += theta_offset / sqrt(r / this->inner_radius);

    while (theta < 0.0 || theta > 2.0 * M_PI) {
      theta = theta > 2.0 * M_PI ? theta - 2.0 * M_PI : theta;
      theta = theta < 0.0 ? theta + 2.0 * M_PI : theta;
    }

    double x = (tx_width - 2) * (theta / 2.0 / M_PI);
    double y = (tx_height - 2) * (r - this->inner_radius) / (this->outer_radius - this->inner_radius);

    assert(x >= 0.0 && x < tx_width);
    assert(y >= 0.0 && y < tx_height);

    this->tx_lookup(x, y, m.color);

    m.opacity = (m.color[0] + m.color[1] + m.color[2]) / (255.0f);
    m.opacity = m.opacity > 1.0f ? 1.0f : m.opacity;

  }


  return true;
}
