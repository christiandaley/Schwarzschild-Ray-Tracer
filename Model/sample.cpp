#include <iostream>
#include <fstream>
#include "Metric.h"
#include "Vector.h"
#include "LightPath.h"
#include "Timer.h"

void sample();

const double dt = 5.0;
const double rs = 100.0;
const double jump_dist = 0.05;
const int INC_PER_STEP = 100;

static const Metric metric = Metric(100.0, Point3d());

int main(int argv, char **argc) {

  sample();

  return 0;
}

void sample() {

  std::ofstream data_file;
  data_file.open(model_path);

  Ray ray;

  unsigned short input[3];
  double output[3];

  Timer t;

  for (int d_step = 0; d_step < MAX_DIST_STEPS; d_step++) {
    for (int alpha_step = 0; alpha_step < MAX_ALPHA_STEPS; alpha_step++) {
      double alpha = alpha_step * D_ALPHA;
      double cos_alpha = cos(DEG_2_RAD(alpha));
      double sin_alpha = sin(DEG_2_RAD(alpha));
      double d = dist[d_step];
      ray.D = Vector3d(cos_alpha, sin_alpha, 0.0);
      ray.O = Vector3d(rs * d, 0.0, 0.0);

      int tmax = MIN(MAX_T_STEPS, (int)((d - 1.0) * STEPS_PER_RADIUS));

      double new_d, beta, gamma;
      for (int t_step = 0; t_step <= tmax; t_step++) {

        new_d = ray.O.dist(metric.origin) / rs;

        beta = atan2(ray.O.y, ray.O.x);
        beta += beta < 0.0 ? 2.0 * M_PI : 0.0;

        gamma = atan2(ray.D.y, ray.D.x);
        gamma += gamma < 0.0 ? 2.0 * M_PI : 0.0;

        input[0] = d_step;
        input[1] = alpha_step;
        input[2] = t_step;
        output[0] = new_d;
        output[1] = beta;
        output[2] = gamma;

        data_file.write((char *)input, sizeof(input));
        data_file.write((char *)output, sizeof(output));


        for (int i = 0; i < INC_PER_STEP; i++) {
          metric.step(jump_dist, ray);
        }
      }
    }


  }

  t.stop();
  data_file.close();
  std::cout << "Generated light paths in ";
  t.print_elapsed_time();

}
