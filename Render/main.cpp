#include <iostream>
#include "Scene.h"
#include "Sphere.h"
#include "Disk.h"
#include "RayTracer.h"
#include "PostProcessor.h"
#include "LightPath.h"
#include "Timer.h"
#include "NiceScenes.h"

void simple_scene1();
void einstein_ring1();
void accretion_disk1();

int main(int argc, char **argv) {
  srand(time(NULL));
  //simple_scene1();
  //einstein_ring1();
  //accretion_disk1();
  //planets1();
  //orbit1();
  //orbit2();
  //orbit3();
  //orbit4();
  //orbit5();
  accretion_disk2();
  //accretion_disk3();
  //accretion_disk4();

  return 0;
}

void accretion_disk1() {
  const double fov = 45.0;
  Scene s = Scene();
  Point3d pos = Point3d(0, 2.5, 85);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);

  Camera camera = Camera(pos, dir, up, 500, 500, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 5.0;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  bg.material.ambient = 0.5f;

  Disk disk = Disk(Point3d(), UP, RIGHT, 12.5, 40.0, "Textures/adisk3.png");

  s.addObject(bg);
  s.addObject(disk);

  RayTracer::load_scene(s);

  Timer t = Timer();
  RayTracer::render(0.05, 29.9, EULER_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/Disk1/disk_euler.png");

  LightPath::LoadModel();

  t.reset();
  RayTracer::render(0.05, 29.9, ACCEL_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/Disk1/disk_accel.png");
}

void einstein_ring1() {
  const double fov = 25.0;
  Scene s = Scene();
  Point3d pos = Point3d(0, 0.0, 85);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);

  Camera camera = Camera(pos, dir, up, 500, 500, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 5.0;

  Background bg = Background(Point3d(), 1e9, "Textures/stars1.png");
  bg.setAxis(Vector3d(-0.1, 0.0, 1.0), Vector3d(1.0, 0.0, 0.0));
  bg.material.ambient = 0.5f;

  Sphere star = Sphere(Point3d(0.0, 0.0, 10.0), 1.5, "Textures/sun2k.png");

  s.addObject(bg);
  s.addObject(star);

  RayTracer::load_scene(s);

  Timer t = Timer();
  RayTracer::render(0.05, 29.9, EULER_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/ERing1/ring_euler1.png");

  LightPath::LoadModel();

  t.reset();
  RayTracer::render(0.05, 29.9, ACCEL_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/ERing1/ring_accel.png");

  t.reset();
  RayTracer::render(1.5, 29.9, EULER_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();
  RayTracer::write("Renders/ERing1/ring_euler2.png");

}

void simple_scene1() {
  const double fov = 45.0;
  Scene s = Scene();
  Point3d pos = Point3d(0, 2.5, 85);
  Vector3d dir = Vector3d(-0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);

  Camera camera = Camera(pos, dir, up, 500, 500, 10.0, fov);
  camera.xsamples = 1;
  camera.ysamples = 1;
  s.camera = camera;

  Background bg = Background(Point3d(), 1e9, "Textures/stars1.png");
  bg.setAxis(Vector3d(-0.1, 0.0, 1.0), Vector3d(1.0, 0.0, 0.0));

  s.addObject(bg);
  RayTracer::load_scene(s);

  Timer t = Timer();
  RayTracer::render(1e20, 29.9, EULER_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/SimpleScene1/image1.png");
  s.metric.rs = 3.0;
  t.reset();

  RayTracer::render(0.05, 29.9, EULER_RAY_TRACE, 1);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  RayTracer::write("Renders/SimpleScene1/image2.png");

}
