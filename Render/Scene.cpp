#include <png++/png.hpp>
#include <cstdlib>
#include <ctime>
#include "Scene.h"
#include "LightPath.h"

void Scene::addObject(Object &obj) {
  this->objects.push_back(&obj);
  this->n_objects = this->objects.size();
}

void Scene::setMetric(const Metric &m) {
  this->metric = m;
}

double Scene::get_smallest_radius() const {
  double min = 1e20;
  for (std::vector<Object *>::const_iterator it = this->objects.begin(); it != this->objects.end(); it++) {
    const double d = (*it)->dist_from_point(this->metric.origin);
    min = d < min ? d : min;
  }

  return min;
}
