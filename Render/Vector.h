#pragma once

#include <iostream>
#include <math.h>

#define EPSILON 0.00001
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define DEG_2_RAD(theta) ((theta) * M_PI / 180.0)
#define RAD_2_DEG(theta) ((theta) * 180.0 / M_PI)

#define UP Vector3d(0.0, 1.0, 0.0)
#define RIGHT Vector3d(1.0, 0.0, 0.0)
#define IN Vector3d(0.0, 0.0, -1.0)

typedef float Color[3];

class Vector3d {
public:
  double x, y, z;
  Vector3d() { x = 0.0, y = 0.0, z = 0.0; }
  Vector3d(const double x, const double y, const double z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  Vector3d operator+(const Vector3d &v) const {
    return Vector3d(x + v.x, y + v.y, z + v.z);
  }

  Vector3d operator-(const Vector3d &v) const {
    return Vector3d(x - v.x, y - v.y, z - v.z);
  }

  Vector3d operator-() const {
    return Vector3d(-x, -y, -z);
  }

  double operator*(const Vector3d &v) const {
    return x * v.x + y * v.y + z * v.z;
  }

  Vector3d operator*(const double c) const {
    return Vector3d(x * c, y * c, z * c);
  }

  bool operator==(const Vector3d &v) const {
    return (*this - v).mag() <= EPSILON;
  }

  bool operator!=(const Vector3d &v) const {
    return !(*this == v);
  }

  Vector3d cross(const Vector3d &v) const {
    return Vector3d( y * v.z - z * v.y,
                     z * v.x - x * v.z,
                     x * v.y - y * v.x);
  }


  Vector3d normalize() const {
    return *this * (1.0 / mag());
  }

  double dist(const Vector3d &v) const {
    return (*this - v).mag();
  }

  double dist2(const Vector3d &v) const {
    return (*this - v).mag2();
  }

  double mag2() const {
    return x * x + y * y + z * z;
  }

  double mag() const {
    return sqrt(x * x + y * y + z * z);
  }

  void print() const {
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
  }

  void print_norm() const {
    std::cout << this->mag() << std::endl;
  }

  static Vector3d rand_unit_vec_in_hemisphere(const Vector3d &v) {
  	Vector3d orth;

  	double theta = M_PI / 2.0 * ((double)rand() / RAND_MAX);
  	double phi = 2.0 * M_PI * ((double)rand() / RAND_MAX);

  	// orth.x * v.x + orth.y * v.y + orth.z * v.Z = 0
  	// we let orth.y and orth.z = 1
  	// orth.x * v.x + v.y + v.z = 0
  	// orth.x = -(v.y + v.z) / v.x
  	if (v.x != 0.0) {
  		orth.x = -(v.y + v.z) / v.x;
  		orth.y = 1.0;
  		orth.z = 1.0;
  	}
  	else if (v.y != 0.0)
  	{
  		orth.y = -(v.x + v.z) / v.y;
  		orth.x = 1.0;
  		orth.z = 1.0;
  	}
  	else if (v.z != 0.0) {
  		orth.z = -(v.x + v.y) / v.z;
  		orth.x = 1.0;
  		orth.y = 1.0;
  	}

  	orth = orth.normalize();
  	Vector3d cross = v.cross(orth);

  	Vector3d projX = orth * sin(theta) * cos(phi);
  	Vector3d projY = cross * sin(theta) * sin(phi);
  	Vector3d projZ = v * cos(theta);

  	return projX + projY + projZ;
  }

  static Vector3d rand_weighted_unit_vec_in_hemisphere(const Vector3d &v) {
  	double theta = asin((double)rand() / RAND_MAX);
  	double phi = 2.0 * M_PI * ((double)rand() / RAND_MAX);

  	Vector3d orth;
  	// orth.x * v.x + orth.y * v.y + orth.z * v.Z = 0
  	// we let orth.y and orth.z = 1
  	// orth.x * v.x + v.y + v.z = 0
  	// orth.x = -(v.y + v.z) / v.x
  	if (v.x != 0.0) {
  		orth.x = -(v.y + v.z) / v.x;
  		orth.y = 1.0;
  		orth.z = 1.0;
  	}
  	else if (v.y != 0.0)
  	{
  		orth.y = -(v.x + v.z) / v.y;
  		orth.x = 1.0;
  		orth.z = 1.0;
  	} else if (v.z != 0.0) {
  		orth.z = -(v.x + v.y) / v.z;
  		orth.x = 1.0;
  		orth.y = 1.0;
  	}

  	orth = orth.normalize();
  	Vector3d cross = v.cross(orth);
  	cross.normalize();

  	Vector3d projX = orth * sin(theta) * cos(phi);
  	Vector3d projY = cross * sin(theta) * sin(phi);
  	Vector3d projZ = v * cos(theta);


  	return projX + projY + projZ;
  }

};

typedef Vector3d Point3d;

class Ray {
public:
  Point3d O;
  Vector3d D;
  Ray(const Point3d &O, const Vector3d &D) {
    this->O = O;
    this->D = D;
  }

  Ray() {
    this->O = Point3d();
    this->D = Vector3d();
  }
  void print() const {
    std::cout << "[";
    this->O.print();
    this->D.print();
    std::cout << "]\n";
  }
};
