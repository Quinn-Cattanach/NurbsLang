#include "primitives.h"

nurbs<2> rectangle(float width, float height) {
  return hyperbox<2>({width, height});
}

nurbs<3> box(float width, float height, float depth) {
  return hyperbox<3>({width, height, depth});
}

nurbs<1> line(float length, vec3f_wgsl direction) {
  nurbs<1> result;

  const float nrm = direction.norm();
  if (nrm != 0.0)
    direction /= nrm;

  result.degree[0] = 1;
  result.knot[0] = {0.0, 0.0, 1.0, 1.0};

  result.control.reserve(2);
  result.weight.reserve(2);

  result.control.emplace_back(0.0, 0.0, 0.0);
  result.weight.emplace_back(1.0);

  vec3f_wgsl end = length * direction;
  result.control.emplace_back(end.x, end.y, end.z);
  result.weight.emplace_back(1.0);

  return result;
}

nurbs<1> bent_line(float length, float bend_origin_u, float radius,
                   vec3f_wgsl base_dir, vec3f_wgsl bend_dir) {

  /**
   * Ok so for simplicity this is basically copying bezier path because we are
   * out of time for a true bend function </3
   */

  nurbs<1> result;
  result.degree[0] = 3;

  float n0 = base_dir.norm();
  float n1 = bend_dir.norm();
  if (n0 != 0.0f)
    base_dir /= n0;
  if (n1 != 0.0f)
    bend_dir /= n1;

  float cos_angle = clamp(base_dir.dot(bend_dir), -1.0f, 1.0f);
  float angle = acosf(cos_angle);

  if (angle < 1e-6f || angle > 3.14159f - 1e-6f) {
    angle = 0.0f;
    radius = 0.0f;
  }

  vec3f_wgsl bend_dir_ortho = bend_dir - base_dir * base_dir.dot(bend_dir);
  float n2 = bend_dir_ortho.norm();
  if (n2 > 1e-6f) {
    bend_dir_ortho /= n2;
  } else {
    if (fabs(base_dir.x) < fabs(base_dir.y))
      bend_dir_ortho =
          vec3f_wgsl(1, 0, 0) - base_dir * base_dir.dot(vec3f_wgsl(1, 0, 0));
    else
      bend_dir_ortho =
          vec3f_wgsl(0, 1, 0) - base_dir * base_dir.dot(vec3f_wgsl(0, 1, 0));
    float n3 = bend_dir_ortho.norm();
    if (n3 != 0.0f)
      bend_dir_ortho /= n3;
    else
      bend_dir_ortho = vec3f_wgsl(0, 0, 1);
  }

  bend_origin_u = clamp(bend_origin_u, 0.0f, 1.0f);

  float arc_len = radius * angle;

  // The center of the arc is at bend_origin_u along the total length
  // The arc starts radius distance BEFORE the center
  // The arc ends radius distance AFTER the center (measured along the chord)

  float center_position = bend_origin_u * length;
  float pre_len = center_position - radius * sinf(angle / 2.0f);
  float post_len = length - center_position - radius * sinf(angle / 2.0f);

  if (pre_len < 0.0f)
    pre_len = 0.0f;
  if (post_len < 0.0f)
    post_len = 0.0f;

  vec3f_wgsl p0 = vec3f_wgsl(0.0f, 0.0f, 0.0f);
  vec3f_wgsl p1 = p0 + base_dir * pre_len;

  vec3f_wgsl end_tangent =
      base_dir * cosf(angle) + bend_dir_ortho * sinf(angle);

  vec3f_wgsl arc_center =
      p1 + base_dir * radius +
      bend_dir_ortho * radius * (1.0f - cosf(angle)) / sinf(angle);
  vec3f_wgsl p2 =
      p1 +
      base_dir * (2.0f * radius * sinf(angle / 2.0f) * cosf(angle / 2.0f)) +
      bend_dir_ortho *
          (2.0f * radius * sinf(angle / 2.0f) * sinf(angle / 2.0f));

  vec3f_wgsl p3 = p2 + end_tangent * post_len;
  float k = 4.0f / 3.0f * tanf(angle / 4.0f);

  vector<vec3f_wgsl> C;
  C.reserve(10);
  C.push_back(p0);
  C.push_back(p0 + base_dir * (pre_len / 3.0f));
  C.push_back(p0 + base_dir * (2.0f * pre_len / 3.0f));
  C.push_back(p1);
  C.push_back(p1 + base_dir * (k * radius));
  C.push_back(p2 - end_tangent * (k * radius));
  C.push_back(p2);
  C.push_back(p2 + end_tangent * (post_len / 3.0f));
  C.push_back(p2 + end_tangent * (2.0f * post_len / 3.0f));
  C.push_back(p3);

  result.control.clear();
  result.weight.clear();
  for (auto &pt : C) {
    result.control.emplace_back(pt.x, pt.y, pt.z);
    result.weight.emplace_back(1.0f);
  }

  float L = pre_len + arc_len + post_len;
  float t1 = 1.0f / 3.0f, t2 = 2.0f / 3.0f;
  if (L > 1e-9f) {
    t1 = pre_len / L;
    t2 = (pre_len + arc_len) / L;
    t1 = clamp(t1, 0.0f, 1.0f);
    t2 = clamp(t2, 0.0f, 1.0f);
  }

  result.knot[0].clear();
  result.knot[0].insert(result.knot[0].end(), 4, 0.0f);
  result.knot[0].insert(result.knot[0].end(), 3, t1);
  result.knot[0].insert(result.knot[0].end(), 3, t2);
  result.knot[0].insert(result.knot[0].end(), 4, 1.0f);

  return result;
}
