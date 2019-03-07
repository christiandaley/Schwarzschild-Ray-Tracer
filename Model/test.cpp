#include <iostream>
#include <fstream>
#include <assert.h>
#include "Metric.h"
#include "Vector.h"
#include "LightPath.h"
#include "Timer.h"

void test(int n_trials);

int main(int argc, char **argv) {
  int n_trials = 10000;
  if (argc > 1) {
    n_trials = atoi(argv[1]);
  }

  assert(n_trials > 0);

  test(n_trials);
  return 0;
}

void test(const int n_trials) {
  const double rs = 100.0;
  Metric metric = Metric(rs, Point3d());
  srand(time(NULL));

  assert(n_trials > 0);
  std::cout << "Performing " << n_trials << " randomized tests..." << std::endl;

  double d, alpha, t;
  double avg1 = 0.0;
  double avg2 = 0.0;
  double var1 = 0.0;
  double var2 = 0.0;
  double min1 = 1e20;
  double min2 = 1e20;
  double max1 = 0.0;
  double max2 = 0.0;

  LightPath::LoadModel();

  Timer t1, t2, t3;
  double total_dist = 0.0;
  unsigned int total_steps = 0;

  for (int i = 0; i < n_trials; i++) {
    d = ((double)rand() / RAND_MAX) * (30.0 - 1.05) + 1.05;
    alpha = ((double)rand() / RAND_MAX) * M_PI;
    t = ((double)rand() / RAND_MAX) * (d - 1.0) * rs;

    Ray ray1 = Ray(Point3d(rs * d, 0, 0), Vector3d(cos(alpha), sin(alpha), 0));
    Ray ray2 = ray1;
    Ray ray3 = ray1;

    const double elapsed = t1.elapsed;
    t1.start();
    // If the model couldn't make the jump we just pick new inputs and try again
    if (!LightPath::ModelQuery(ray1, t, metric)) {
      t1.stop();
      t1.elapsed = elapsed;
      i--;
      continue;
    }

    t1.stop();

    t2.start();
    LightPath::MetricQuery(ray2, 10.0, t, metric);
    t2.stop();

    total_steps += (int)ceil(t / 10.0);

    t3.start();
    LightPath::MetricQuery(ray3, 0.05, t, metric);
    t3.stop();

    total_dist += t;

    const double e1 = ray1.O.dist(ray3.O) / t;
    const double e2 = ray2.O.dist(ray3.O) / t;

    avg1 += e1;
    var1 += e1 * e1;
    avg2 += e2;
    var2 += e2 * e2;

    max1 = e1 > max1 ? e1 : max1;
    min1 = e1 < min1 ? e1 : min1;
    max2 = e2 > max2 ? e2 : max2;
    min2 = e2 < min2 ? e2 : min2;

  }

  avg1 /= n_trials;
  avg2 /= n_trials;
  var1 /= n_trials;
  var2 /= n_trials;

  std::cout << "Finished running tests" << std::endl;

  std::cout << "Results for model:\n";

  std::cout << "  Average error/distance: " << avg1 << std::endl;
  std::cout << "  Variance:               " << var1 - avg1 * avg1 << std::endl;
  std::cout << "  Maximum error:          " << max1 << std::endl;
  std::cout << "  Minimum error:          " << min1 << std::endl;
  std::cout << "  Total time:             " << t1.milliseconds() << " ms" << std::endl;
  std::cout << "  ns/trial:               " << t1.nanoseconds() / n_trials << std::endl;


  std::cout << "Results for step size of 10:\n";

  std::cout << "  Average error/distance: " << avg2 << std::endl;
  std::cout << "  Variance:               " << var2 - avg2 * avg2 << std::endl;
  std::cout << "  Maximum error:          " << max2 << std::endl;
  std::cout << "  Minimum error:          " << min2 << std::endl;
  std::cout << "  Total time:             " << t2.milliseconds() << " ms" << std::endl;
  std::cout << "  ns/trial:               " << t2.nanoseconds() / n_trials << std::endl;
  std::cout << "  Total steps:            " << total_steps << std::endl;
  std::cout << "  ns/step:                " << t2.nanoseconds() / total_steps << std::endl;

  //t1.print_elapsed_time();
  //t2.print_elapsed_time();
  //t3.print_elapsed_time();

  std::cout << "On average..." << std::endl;
  std::cout << "The model produced an error " << (avg1 / avg2) << " times larger" << std::endl;
  std::cout << "The model was " << (t2.nanoseconds() / t1.nanoseconds()) << " times faster" << std::endl;
  std::cout << "Average jump distance was " << total_dist / n_trials << std::endl;
}
