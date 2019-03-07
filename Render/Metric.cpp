#include <assert.h>
#include "Metric.h"


Metric::Metric(const double rs, const Vector3d &origin) {
  this->rs = rs;
  this->origin = origin;
}

Metric::Metric() {
  this->rs = 0.0;
  this->origin = Point3d();
}

double Metric::get_step_size(const double dt, const Ray &ray) const {
  const Point3d O = ray.O - this->origin;
  const double r = O.mag();
  const Vector3d R = O * (1.0 / r);
  const double cos2_alpha = (R * ray.D) * (R * ray.D);
  const double sin2_alpha = 1.0 - cos2_alpha;

  double step;

  if (sin2_alpha == 0.0) {
    step = 1e20;
  } else {
    step = dt * (1.0 + sqrt(cos2_alpha / sin2_alpha)) * r / this->rs;
  }

  return MIN(step, r - this->rs + EPSILON);
}

bool Metric::step(const double dt, Ray &ray) const {
  Ray r;
  return this->step(dt, ray, r);
}

bool Metric::step(const double dt, Ray &ray, Ray &step_ray) const {
  const Vector3d O = ray.O - this->origin;

  if (O * O <= this->rs * this->rs) {
    return false;
  }

  const double r = O.mag();
  const Vector3d R = O * (1.0 / r);

  const double cos2_alpha = (R * ray.D) * (R * ray.D);
  const double cot2_alpha = cos2_alpha / (1.0 - cos2_alpha);

  const double k = (r - this->rs) * cot2_alpha + r;

  const double accel = -3.0 * this->rs / 2.0 / k / k;

  Vector3d newD = (ray.D + R * (accel * dt)).normalize();

  step_ray.D = (ray.D + newD).normalize();
  step_ray.O = ray.O;

  ray.O = ray.O + step_ray.D * dt;
  ray.D = newD;

  return true;
}

void Metric::timelike_step(const double time, const double dt, Ray &ray) const {
  const Point3d O = ray.O - this->origin;
  double r = O.mag();
  const Vector3d X = O * (1.0 / r);
  const Vector3d Y = (abs(ray.D * X) == 1.0) ? Vector3d() : (ray.D - X * (X * ray.D)).normalize();

  double rdot = ray.D * X;
  double phidot = (ray.D * Y) / r;
  double phi = 0.0;
  const int iter = (int)(time / dt);

  for (int i = 0; i < iter; i++) {
    const double rrs = 1.0 - this->rs / r;

    const double rddot = -this->rs * rrs / (2.0 * r * r) + 3.0 * this->rs * rdot * rdot / (2.0 * r * r * rrs) + (r - this->rs) * phidot * phidot;
    const double phiddot = -2.0 * rdot * phidot / r + this->rs * rdot * phidot / (r * r * rrs);

    const double new_rdot = rdot + rddot * dt;
    const double new_phidot = phidot + phiddot * dt;

    r += (rdot + new_rdot) * dt / 2.0;
    phi += (phidot + new_phidot) * dt / 2.0;

    rdot = new_rdot;
    phidot = new_phidot;
  }

  ray.O = this->origin + X * (cos(phi) * r) + Y * (sin(phi) * r);
  ray.D = X * (rdot * cos(phi) - r * phidot * sin(phi)) + Y * (rdot * sin(phi) + r * phidot * cos(phi));

}
