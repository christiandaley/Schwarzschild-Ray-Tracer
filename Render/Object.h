#pragma once

#include "Vector.h"

struct Material {
  // the color of the object at that point
  Color color = {0.0f, 0.0f, 0.0f};
  // ambient lighting
  float ambient = 1.0f;
  // diffuse lighting
  float diffuse = 0.0f;
  // specular lighting
  float specular = 0.0f;
  // the base opacity
  float opacity = 1.0f;
  Vector3d normal;
  Vector3d velocity;
  Vector3d lighting;
};

class Object {
public:
  Material material;
  bool hasTexture = false;
  int tx_width, tx_height;
  Color *bmp = NULL;
  void load_tx(const char *filename);
  void tx_lookup(const double x, const double y, Color color) const;
  virtual bool isBackground() const { return false; }
  virtual double dist_from_point(const Point3d &p) const = 0;
  virtual bool intersects(const Ray &ray, const double max_t, double &t, Material &m) const = 0;

  virtual ~Object();
};
