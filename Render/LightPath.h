#pragma once
#include "Vector.h"
#include "Metric.h"

#define D_ALPHA 1.0

// the step in normalized time
#define STEPS_PER_RADIUS 20.0

#define MAX_ALPHA_STEPS 181
#define MAX_DIST_STEPS 79
#define MAX_T_STEPS ((int)(30 * STEPS_PER_RADIUS))

extern const double dist[];
extern const char *model_path;

class LightPath {
  static double tstep;
  static double rs;
  static double ***buffer;
  static int get_dist_steps(const double d);
public:
  static void LoadModel();
  static bool MetricQuery(Ray &ray, const double dt, const double tfinal, const Metric &metric);
  static bool ModelQuery(Ray &ray, double &jump_dist, const Metric &metric);


};
