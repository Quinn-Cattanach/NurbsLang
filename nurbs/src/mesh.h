#pragma once
#include <gismo.h>
#include <string>
#include <vector>

using namespace gismo;

struct vec3f_wgsl {
  float x, y, z;
  float _padding;

  vec3f_wgsl() : x(0), y(0), z(0), _padding(0) {}
  vec3f_wgsl(float x_, float y_, float z_) : x(x_), y(y_), z(z_), _padding(0) {}

  float norm() { return sqrtf(x * x + y * y + z * z); }

  vec3f_wgsl operator-() const { return vec3f_wgsl(-x, -y, -z); }

  vec3f_wgsl operator+(const vec3f_wgsl &rhs) const {
    return vec3f_wgsl(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  vec3f_wgsl &operator+=(const vec3f_wgsl &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  vec3f_wgsl operator-(const vec3f_wgsl &rhs) const {
    return vec3f_wgsl(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  vec3f_wgsl &operator-=(const vec3f_wgsl &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  vec3f_wgsl operator*(float s) const {
    return vec3f_wgsl(x * s, y * s, z * s);
  }

  friend vec3f_wgsl operator*(float s, const vec3f_wgsl &v) {
    return vec3f_wgsl(v.x * s, v.y * s, v.z * s);
  }

  vec3f_wgsl &operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  vec3f_wgsl operator/(float s) const {
    return vec3f_wgsl(x / s, y / s, z / s);
  }

  vec3f_wgsl &operator/=(float s) {
    x /= s;
    y /= s;
    z /= s;
    return *this;
  }

  bool operator==(const vec3f_wgsl &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
  }

  bool operator!=(const vec3f_wgsl &rhs) const { return !(*this == rhs); }

  float dot(const vec3f_wgsl &rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  vec3f_wgsl cross(const vec3f_wgsl &rhs) const {
    return vec3f_wgsl(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
                      x * rhs.y - y * rhs.x);
  }

  float length() const { return std::sqrt(x * x + y * y + z * z); }

  vec3f_wgsl normalized() const {
    float len = length();
    if (len == 0.0f)
      return vec3f_wgsl(0, 0, 0);
    return *this / len;
  }
};

struct mesh {
  std::vector<vec3f_wgsl> vertices;
  std::vector<vec3f_wgsl> parametricCoordinates;
  std::vector<vec3f_wgsl> normals;

  void to_stl(const std::string &filename) const;

  void compute_normals();

  void append(const mesh &other);
};
