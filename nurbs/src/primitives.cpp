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

nurbs<1> arc(float radius, float angle, vec3f_wgsl center, vec3f_wgsl start_dir,
             vec3f_wgsl normal) {
  start_dir = start_dir.normalized();
  normal = normal.normalized();
  vec3f_wgsl binormal = start_dir.cross(normal).normalized();

  int num_segments;
  if (angle <= M_PI / 2.0) {
    num_segments = 1; // 0-90 degrees: 1 segment
  } else if (angle <= M_PI) {
    num_segments = 2; // 90-180 degrees: 2 segments
  } else if (angle <= 3.0 * M_PI / 2.0) {
    num_segments = 3; // 180-270 degrees: 3 segments
  } else {
    num_segments = 4; // 270-360 degrees: 4 segments
  }

  float segment_angle = angle / num_segments;

  nurbs<1> result;
  result.degree[0] = 2;

  int num_control_points = 2 * num_segments + 1;
  result.control.reserve(num_control_points);
  result.weight.reserve(num_control_points);

  // Build each segment
  for (int seg = 0; seg < num_segments; seg++) {
    float theta0 = seg * segment_angle;
    float theta1 = (seg + 1) * segment_angle;
    float theta_mid = (theta0 + theta1) / 2.0;

    // Calculate the weight for the middle control point
    float w_mid = cos(segment_angle / 2.0);

    vec3f_wgsl p0 = center + start_dir * (radius * cos(theta0)) +
                    binormal * (radius * sin(theta0));
    vec3f_wgsl tangent0 = start_dir * (-sin(theta0)) + binormal * cos(theta0);
    vec3f_wgsl tangent1 = start_dir * (-sin(theta1)) + binormal * cos(theta1);
    vec3f_wgsl p1 =
        center + (start_dir * cos(theta_mid) + binormal * sin(theta_mid)) *
                     (radius / w_mid);
    vec3f_wgsl p2 = center + start_dir * (radius * cos(theta1)) +
                    binormal * (radius * sin(theta1));

    // Add control points and weights
    if (seg == 0) {
      // First segment: add all three points
      result.control.push_back(p0);
      result.weight.push_back(1.0);

      result.control.push_back(p1);
      result.weight.push_back(w_mid);

      result.control.push_back(p2);
      result.weight.push_back(1.0);
    } else {
      // Subsequent segments: first point is shared, add middle and end
      result.control.push_back(p1);
      result.weight.push_back(w_mid);

      result.control.push_back(p2);
      result.weight.push_back(1.0);
    }
  }

  // Build knot vector
  result.knot[0].clear();

  // Starting knots (degree + 1 = 3 knots at 0)
  for (int i = 0; i <= result.degree[0]; i++) {
    result.knot[0].push_back(0.0);
  }

  // Internal knots (at segment boundaries, multiplicity 2)
  for (int seg = 1; seg < num_segments; seg++) {
    float knot_value = (float)seg / (float)num_segments;
    result.knot[0].push_back(knot_value);
    result.knot[0].push_back(knot_value); // multiplicity 2
  }

  // Ending knots (degree + 1 = 3 knots at 1)
  for (int i = 0; i <= result.degree[0]; i++) {
    result.knot[0].push_back(1.0);
  }

  return result;
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
