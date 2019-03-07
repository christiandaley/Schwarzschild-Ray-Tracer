#include <iostream>
#include <assert.h>
#include <fstream>
#include "LightPath.h"

double ***LightPath::buffer = NULL;

const double dist[] = {
            30.0, 29.0, 28.0, 27.0, 26.0, 25.0, 24.0, 23.0, 22.0, 21.0,
            20.0, 19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 13.0, 12.0, 11.0,
            10.0, 9.5, 9.0, 8.5, 8.0, 7.5, 7.0, 6.75, 6.5, 6.25,
            6.0, 5.75, 5.5, 5.25, 5.0, 4.8, 4.6, 4.4, 4.2, 4.0,
            3.9, 3.8, 3.7, 3.6, 3.5, 3.4, 3.3, 3.2, 3.1, 3.0,
            2.9, 2.8, 2.7, 2.6, 2.5, 2.4, 2.3, 2.2, 2.1, 2.0,
            1.95, 1.9, 1.85, 1.8, 1.75, 1.7, 1.65, 1.6, 1.55, 1.5,
            1.45, 1.4, 1.35, 1.3, 1.25, 1.2, 1.15, 1.1, 1.05};

const char *model_path = "../Model/Data/model.dat";



int LightPath::get_dist_steps(const double d) {
  int steps = 0;
  /*for (; dist[steps] >= d; steps++) {
    if (steps > MAX_DIST_STEPS) {
      assert(false);
    }
  }
  return steps - 1;*/

  // binary search
  int min = 0;
  int max = MAX_DIST_STEPS - 1;
  do {
    steps = (min + max) / 2;
    if (dist[steps+1] > d)
      min = steps;
    else if (dist[steps] < d)
      max = steps;
    else
      break;

  } while (true);

  return steps;
}

void LightPath::LoadModel() {

  assert(buffer == NULL);
  std::ifstream data_file;

  data_file.open(model_path);

  int max_t_step;

  buffer = new double **[MAX_DIST_STEPS];
  for (int i = 0; i < MAX_DIST_STEPS; i++) {
    buffer[i] = new double *[MAX_ALPHA_STEPS];
    for (int j = 0; j < MAX_ALPHA_STEPS; j++) {
      max_t_step = 1 + (dist[i] - 1.0) * STEPS_PER_RADIUS;
      buffer[i][j] = new double[max_t_step * 3];
    }
  }

  int d_step, alpha_step, t_step;
  double new_d, beta, gamma;
  unsigned short input[3];
  double output[3];

  while (!data_file.eof()) {
    data_file.read((char *)input, sizeof(input));
    data_file.read((char *)output, sizeof(output));
    d_step = input[0];
    alpha_step = input[1];
    t_step = input[2];
    new_d = output[0];
    beta = output[1];
    gamma = output[2];

    max_t_step = 1 + (dist[d_step] - 1.0) * STEPS_PER_RADIUS;

    assert(alpha_step >= 0 && alpha_step < MAX_ALPHA_STEPS);
    assert(d_step >= 0 && d_step < MAX_DIST_STEPS);
    assert(t_step >= 0 && t_step < max_t_step);
    buffer[d_step][alpha_step][t_step * 3] = new_d;
    buffer[d_step][alpha_step][t_step * 3 + 1] = beta;
    buffer[d_step][alpha_step][t_step * 3 + 2] = gamma;

  }

  data_file.close();
}

bool LightPath::ModelQuery(Ray &ray, double &jump_dist, const Metric &metric) {

  const double r = ray.O.dist(metric.origin);

  if (r / metric.rs <= dist[MAX_DIST_STEPS - 1]) {
    return false;
  }

  const Vector3d R = (ray.O - metric.origin) * (1.0 / r);


  if (ray.D * R == 1.0) {
    ray.O = ray.O + ray.D * jump_dist;
    return true;
  } else if (ray.D * R == -1.0) {
    // Make sure we can't jump through the event horizon
    jump_dist = MIN(jump_dist, r - metric.rs + EPSILON);
    ray.O = ray.O + ray.D * jump_dist;
    return true;
  }

  const double d = r / metric.rs;
  const int d_step = get_dist_steps(d);
  const double alpha = RAD_2_DEG(acos(ray.D * R));
  const int alpha_step = (int)(alpha / D_ALPHA);

  assert(d_step >= 0 && d_step < MAX_DIST_STEPS - 1);
  assert(alpha_step >= 0 && alpha_step < MAX_ALPHA_STEPS - 1);

  jump_dist = MIN(jump_dist, r - metric.rs);

  int t_step = (int)((jump_dist / metric.rs) * STEPS_PER_RADIUS);
  const int max_t_step = MIN(MAX_T_STEPS, (int)((dist[d_step+1] - 1.0) * STEPS_PER_RADIUS));

  t_step = MIN(t_step, max_t_step) - 1;

  if (t_step <= 0) {
    return false;
  }

  jump_dist = ((double)t_step / STEPS_PER_RADIUS) * metric.rs;
  //assert(jump_dist > 0.0);


  const double alpha_weight = 1.0 - (alpha - (alpha_step * D_ALPHA)) / D_ALPHA;
  const double d_weight = 1.0 - (dist[d_step] - d) / (dist[d_step] - dist[d_step+1]);

  const double w1 = d_weight * alpha_weight;
  const double w2 = (1.0 - d_weight) * alpha_weight;
  const double w3 = d_weight * (1.0 - alpha_weight);
  const double w4 = (1.0 - d_weight) * (1.0 - alpha_weight);



  const double new_d = w1 * buffer[d_step][alpha_step][t_step * 3]
                    +  w2 * buffer[d_step+1][alpha_step][t_step * 3]
                    +  w3 * buffer[d_step][alpha_step+1][t_step * 3]
                    +  w4 * buffer[d_step+1][alpha_step+1][t_step * 3];

  const double beta = w1 * buffer[d_step][alpha_step][t_step * 3 + 1]
                    +  w2 * buffer[d_step+1][alpha_step][t_step * 3 + 1]
                    +  w3 * buffer[d_step][alpha_step+1][t_step * 3 + 1]
                    +  w4 * buffer[d_step+1][alpha_step+1][t_step * 3 + 1];

  const double gamma = w1 * buffer[d_step][alpha_step][t_step * 3 + 2]
                    +  w2 * buffer[d_step+1][alpha_step][t_step * 3 + 2]
                    +  w3 * buffer[d_step][alpha_step+1][t_step * 3 + 2]
                    +  w4 * buffer[d_step+1][alpha_step+1][t_step * 3 + 2];

  const double cos_beta = cos(beta);
  const double cos_gamma = cos(gamma);

  const double sin_beta = (beta > M_PI ? -1.0 : 1.0) * sqrt(1.0 - cos_beta * cos_beta);
  const double sin_gamma = (gamma > M_PI ? -1.0 : 1.0) * sqrt(1.0 - cos_gamma * cos_gamma);

  const Vector3d Y = (ray.D - R * (R * ray.D)).normalize();

  ray.O = metric.origin + R * (new_d * metric.rs * cos_beta) + Y * (new_d * metric.rs * sin_beta);
  ray.D = R * cos_gamma + Y * sin_gamma;

  return true;
}

bool LightPath::MetricQuery(Ray &ray, const double dt, const double tfinal, const Metric &metric) {
  double curr_time = 0.0;
  for (; curr_time <= tfinal - dt; curr_time += dt) {
    if (!metric.step(dt, ray))
      return false;
  }

  if (curr_time < tfinal)
    return metric.step(tfinal - curr_time, ray);

  return true;
}
