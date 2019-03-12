#include <iostream>
#include <cstdlib>
#include "NiceScenes.h"
#include "Scene.h"
#include "Sphere.h"
#include "Disk.h"
#include "RayTracer.h"
#include "PostProcessor.h"
#include "LightPath.h"
#include "Timer.h"

static const char *HOME = getenv("HOME");

void accretion_disk4() {
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 1.5, 30.0);
  Vector3d dir = Vector3d(0.5, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;

  const double rs = 5.0;

  s.metric.rs = rs;

  Disk disk = Disk(Point3d(), Vector3d(0.0, 1.0, 0.0), Vector3d(1.0, 0.0, 0.0), rs * 3.0, rs * 10.0, "Textures/adisk3.png");
  Sphere jupiter = Sphere(Point3d(17.0, 3.5, 3.0), 0.75, "Textures/jupiter2k.png");
  jupiter.setAxis(-IN, RIGHT);
  //jupiter.material.ambient = 0.0f;
  //jupiter.material.diffuse = 0.7f;

  s.addObject(disk);
  s.addObject(jupiter);

  LightPath::LoadModel();
  RayTracer::load_scene(s);
  RayTracer::diffuse_samples = 256;

  Timer t;
  RayTracer::render(0.05, 29.9, ACCEL_RAY_TRACE, 32);
  //PostProcessor::Bloom(RayTracer::pixel_buffer, camera.xres, camera.yres, 50.0, 2.5, 4.0);
  std::cout << "Rendered in ";
  t.print_elapsed_time();
  RayTracer::write("Renders/Disk4/disk4-2.png");
}

void accretion_disk3() {
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 0.3, 28.0);
  Vector3d dir = Vector3d(-0.24, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);
  const double fov = 3.0;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;

  const double rs = 1.0;

  s.metric.rs = rs;

  Disk disk = Disk(Point3d(), Vector3d(0.0, 1.0, 0.0), Vector3d(1.0, 0.0, 0.0), rs * 3.0, rs * 10.0, "Textures/adisk3.png");

  const double r = 8.5;
  const double phidot = sqrt(rs / (2.0 * r * r)) / 3.0;
  double phi = 5.0 * M_PI / 4.0;

  Sphere planet = Sphere(Point3d(r * cos(phi), 0.5, -r * sin(phi)), 0.0811, "Textures/neptune2k.png");
  planet.setAxis(-IN, RIGHT);
  planet.material.ambient = 0.0f;
  planet.material.diffuse = 2.5f;
  planet.material.specular = 0.2f;

  s.addObject(disk);
  s.addObject(planet);

  const int fps = 30;
  const int duration = 60;
  const int nf = fps * duration;
  const int offset = 16 * 30;

  LightPath::LoadModel();
  RayTracer::load_scene(s);
  RayTracer::diffuse_samples = 256;

  char filename[128];

  Timer total;
  for (int i = 1; i <= nf; i++) {
    if (i > offset) {
      sprintf(filename, "%s/Desktop/Renders/Disk3/frame%d.png", HOME, i);
      Timer t;

      RayTracer::render(0.0025, 29.9, ACCEL_PATH_TRACE, 32);
      PostProcessor::Bloom(RayTracer::pixel_buffer, camera.xres, camera.yres, 30.0, 1.0, 2.0);

      std::cout << "Rendered frame " << i << " in ";
      t.print_elapsed_time();
      RayTracer::write(filename);
    }

    disk.theta_offset += DEG_2_RAD(8.0 / fps);
    phi += phidot / fps;
    planet.origin = Point3d(r * cos(phi), 0.5, -r * sin(phi));

    if (i < fps * 50) {
      s.camera.dir = s.camera.dir + Vector3d(0.0075 / fps, 0.0, 0.0);
    }

    if (i >= fps * 25 && i < fps * 50) {
      s.camera.verticleFov += 0.25 / fps;
    }

    s.camera.normalize();
  }

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();

}


void accretion_disk2() {
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 1.0, 140.0);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 4096, 2160, 10.0, fov);
  camera.xsamples = 4;
  camera.ysamples = 4;
  s.camera = camera;

  const double rs = 5.0;

  s.metric.rs = rs;

  Disk disk = Disk(Point3d(), Vector3d(-0.1, 1.0, 0.0), Vector3d(1.0, 0.0, 0.0), rs * 3.0, rs * 10.0, "Textures/adisk3.png");

  s.addObject(disk);

  LightPath::LoadModel();
  RayTracer::load_scene(s);

  Timer t;
  RayTracer::render(0.05, 29.9, ACCEL_RAY_TRACE, 32);
  PostProcessor::Bloom(RayTracer::pixel_buffer, camera.xres, camera.yres, 50.0, 0.75, 4.0);
  std::cout << "Rendered in ";
  t.print_elapsed_time();
  RayTracer::write("Renders/Disk2/disk2.png");

}

void orbit5() {
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 4.5, 0.0);
  Vector3d dir = Vector3d(0.0, -1.0, 0.0);
  Vector3d up = Vector3d(0.0, 0.0, -1.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 0.1;

  const double alpha = DEG_2_RAD(100.0);
  const double v = 0.2228412;
  Ray earth_velocity = Ray(Point3d(0.0, 0.0, 0.75), Vector3d(sin(alpha) * v, 0.0, cos(alpha) * v));

  Sphere earth = Sphere(earth_velocity.O, 0.02167, "Textures/earth2k.png");
  earth.setAxis(-IN, RIGHT);
  //earth.material.ambient = 0.0f;
  //earth.material.diffuse = 1.0f;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  bg.material.ambient = 0.5f;

  s.addObject(earth);
  s.addObject(bg);

  const int fps = 30;
  const int duration = 22;
  const int nf = fps * duration;
  const int offset = 0 * fps;



  LightPath::LoadModel();
  RayTracer::load_scene(s);

  char filename[128];
  const double dt = 1e-7;

  for (int i = 1; i <= offset; i++) {
    s.metric.timelike_step(1.0 / fps, dt, earth_velocity);
  }

  earth.origin = earth_velocity.O;

  Timer total;
  for (int i = 1 + offset; i <= nf + offset; i++) {
    sprintf(filename, "%s/Desktop/Renders/Orbit5/frame%d.png", HOME, i);
    Timer t;

    RayTracer::render(0.005, 29.9, ACCEL_RAY_TRACE, 32);
    std::cout << "Rendered frame " << i << " in ";
    t.print_elapsed_time();
    RayTracer::write(filename);

    s.metric.timelike_step(1.0 / fps, dt, earth_velocity);
    earth.origin = earth_velocity.O;
  }

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();
}

void orbit4() {
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 0.0, 1.5);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 0.1;

  const double alpha = DEG_2_RAD(100.0);
  const double v = 0.2228412;
  Ray earth_velocity = Ray(Point3d(0.0, 0.0, 0.75), Vector3d(sin(alpha) * v, 0.0, cos(alpha) * v));

  Sphere earth = Sphere(earth_velocity.O, 0.02167, "Textures/earth2k.png");
  earth.setAxis(-IN, RIGHT);
  //earth.material.ambient = 0.0f;
  //earth.material.diffuse = 1.0f;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  bg.material.ambient = 0.5f;

  s.addObject(earth);
  s.addObject(bg);

  const int fps = 30;
  const int duration = 22;
  const int nf = fps * duration;
  const int offset = 0 * fps;



  LightPath::LoadModel();
  RayTracer::load_scene(s);

  char filename[128];
  const double dt = 1e-7;

  for (int i = 1; i <= offset; i++) {
    s.metric.timelike_step(1.0 / fps, dt, earth_velocity);
  }

  earth.origin = earth_velocity.O;

  Timer total;
  for (int i = 1 + offset; i <= nf + offset; i++) {
    sprintf(filename, "%s/Desktop/Renders/Orbit4/frame%d.png", HOME, i);
    Timer t;

    RayTracer::render(0.005, 29.9, ACCEL_RAY_TRACE, 32);
    std::cout << "Rendered frame " << i << " in ";
    t.print_elapsed_time();
    RayTracer::write(filename);

    s.metric.timelike_step(1.0 / fps, dt, earth_velocity);
    earth.origin = earth_velocity.O;
  }

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();
}

void orbit3() {

  Scene s = Scene();
  Point3d pos = Point3d(0.0, 6.5, 0.0);
  Vector3d dir = Vector3d(0.0, -1.0, 0.0);
  Vector3d up = Vector3d(0.0, 0.0, -1.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 0.1;

  const double alpha = DEG_2_RAD(120.0);
  const double v = 0.21;
  Ray earth_velocity = Ray(Point3d(0.0, 0.0, 1.0), Vector3d(sin(alpha) * v, 0.0, cos(alpha) * v));

  Sphere earth = Sphere(earth_velocity.O, 0.02167, "Textures/earth2k.png");
  earth.setAxis(-IN, RIGHT);
  //earth.material.ambient = 0.0f;
  //earth.material.diffuse = 1.0f;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  bg.material.ambient = 0.5f;

  s.addObject(earth);
  s.addObject(bg);

  const int fps = 30;
  const int duration = 60;
  const int nf = fps * duration;
  const int offset = 0;


  LightPath::LoadModel();
  RayTracer::load_scene(s);

  char filename[128];

  for (int i = 1; i <= offset; i++) {
    s.metric.timelike_step(1.0 / fps, 0.00001, earth_velocity);
  }

  earth.origin = earth_velocity.O;

  Timer total;
  for (int i = 1 + offset; i <= nf + offset; i++) {
    sprintf(filename, "%s/Desktop/Renders/Orbit3/frame%d.png", HOME, i);
    Timer t;
    RayTracer::render(0.005, 29.9, ACCEL_RAY_TRACE, 32);
    std::cout << "Rendered frame " << i << " in ";
    t.print_elapsed_time();
    RayTracer::write(filename);

    s.metric.timelike_step(1.0 / fps, 0.00001, earth_velocity);
    earth.origin = earth_velocity.O;
  }

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();

}

void orbit2() {

  Scene s = Scene();
  Point3d pos = Point3d(0.0, 0.0, 2.0);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);
  const double fov = 22.5;
  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 0.1;

  const double alpha = DEG_2_RAD(120.0);
  const double v = 0.21;
  Ray earth_velocity = Ray(Point3d(0.0, 0.0, 1.0), Vector3d(sin(alpha) * v, 0.0, cos(alpha) * v));

  Sphere earth = Sphere(earth_velocity.O, 0.02167, "Textures/earth2k.png");
  earth.setAxis(-IN, RIGHT);
  //earth.material.ambient = 0.0f;
  //earth.material.diffuse = 1.0f;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  bg.material.ambient = 0.5f;

  s.addObject(earth);
  s.addObject(bg);

  const int fps = 30;
  const int duration = 60;
  const int nf = fps * duration;
  const int offset = 0;


  LightPath::LoadModel();
  RayTracer::load_scene(s);

  char filename[128];

  for (int i = 1; i <= offset; i++) {
    s.metric.timelike_step(1.0 / fps, 0.00001, earth_velocity);
  }

  earth.origin = earth_velocity.O;

  Timer total;
  for (int i = 1 + offset; i <= nf + offset; i++) {
    sprintf(filename, "%s/Desktop/Renders/Orbit2/frame%d.png", HOME, i);
    Timer t;

    RayTracer::render(0.005, 29.9, ACCEL_RAY_TRACE, 32);
    std::cout << "Rendered frame " << i << " in ";
    t.print_elapsed_time();
    RayTracer::write(filename);

    s.metric.timelike_step(1.0 / fps, 0.00001, earth_velocity);
    earth.origin = earth_velocity.O;
  }

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();

}

void orbit1() {
  const double fov = 22.5;
  const double rs = 40.0;
  Scene s = Scene();
  Point3d pos = Point3d(0.0, 0.0, rs * 12.75);
  Vector3d dir = Vector3d(0.0, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);

  Camera camera = Camera(pos, dir, up, 1920, 1080, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = rs;

  Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");

  Sphere sun = Sphere(Point3d(-2.0 * rs, 0.0, 0.0), 2.32, "Textures/sun2k.png");
  sun.material.ambient = 2.5f;

  s.addObject(bg);
  s.addObject(sun);

  RayTracer::load_scene(s);
  LightPath::LoadModel();

  const int fps = 3;
  const int duration = 500;
  const int nf = fps * duration;
  char filename[128];

  double v = sqrt(rs / (2.0 * (sun.origin.mag() - rs)));
  Ray sun_velocity;
  std::cout << v << std::endl;
  sun_velocity.D = Vector3d(0.0, 0.0, v * sqrt(1.0 - rs / sun.origin.mag()));
  sun_velocity.O = sun.origin;

  Timer total;
  for (int i = 1; i <= nf; i++) {
    sprintf(filename, "%s/Desktop/Renders/Orbit1/frame%i.png", HOME, i);
    Timer t;


    RayTracer::render(rs / 100.0, 29.9, ACCEL_RAY_TRACE, 32);
    std::cout << "Rendered frame " << i << " in ";
    t.print_elapsed_time();
    RayTracer::write(filename);

    s.metric.timelike_step(1.0 / fps, 0.0001, sun_velocity);
    sun.origin = sun_velocity.O;
  }

  //sun.origin.print();
  //sun.origin.print_norm();

  std::cout << "Finished rendering frames." << std::endl;
  std::cout << "Total render time: ";
  total.print_elapsed_time();
}

void planets1() {
  const double fov = 25.0;
  Scene s = Scene();
  Point3d pos = Point3d(-9.5, 3.0, 42.0);
  Vector3d dir = Vector3d(-0.4, 0.0, -1.0);
  Vector3d up = Vector3d(0.0, 1.0, 0.0);

  Camera camera = Camera(pos, dir, up, 4096, 2160, 10.0, fov);
  camera.xsamples = 2;
  camera.ysamples = 2;
  s.camera = camera;
  s.metric.rs = 5.0;

  //Background bg = Background(Point3d(), 1e9, "Textures/stars2.png");
  Disk disk = Disk(Point3d(), Vector3d(-0.15, 1.0, 0.0), Vector3d(1.0, 0.0, 0.0), 12.5, 50.0, "Textures/adisk3.png");

  Sphere planet = Sphere(Point3d(-11.0, 3.0, 40.0), 0.25, "Textures/mars2k.png");
  planet.setAxis(-IN, RIGHT);
  planet.material.ambient = 0.0f;
  planet.material.diffuse = 2.5f;

  s.addObject(planet);
  //s.addObject(bg);
  s.addObject(disk);

  RayTracer::load_scene(s);
  RayTracer::diffuse_samples = 256;

  LightPath::LoadModel();

  Timer t;

  RayTracer::render(0.05, 29.9, ACCEL_PATH_TRACE, 32);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  PostProcessor::Bloom(RayTracer::pixel_buffer, camera.xres, camera.yres, 50.0, 2.0, 4.0);
  RayTracer::write("Renders/Planets1/planets1-1.png");

  planet.load_tx("Textures/ice.png");
  planet.material.diffuse = 1.5f;
  planet.material.specular = 0.3f;

  t.reset();
  RayTracer::render(0.05, 29.9, ACCEL_PATH_TRACE, 32);
  std::cout << "Rendered in ";
  t.print_elapsed_time();

  PostProcessor::Bloom(RayTracer::pixel_buffer, camera.xres, camera.yres, 50.0, 2.0, 4.0);
  RayTracer::write("Renders/Planets1/planets1-2.png");

}
