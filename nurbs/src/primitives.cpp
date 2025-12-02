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

    // Normalize directions
    base_dir = base_dir.normalized();
    bend_dir = bend_dir.normalized();

    // Compute bend angle
    float cosA  = clamp(base_dir.dot(bend_dir), -1.0f, 1.0f);
    float angle = acosf(cosA);

    if (angle < 1e-6f)
        throw std::runtime_error("Bent_line: zero bend angle");

    // Compute arc length
    float arc_len = radius * angle;

    // Compute how much straight length remains
    float straight_len = length - arc_len;
    if (straight_len < 0.0f)
        straight_len = 0.0f;

    // Divide straight into two parts
    float pre_len  = bend_origin_u * straight_len;
    float post_len = straight_len - pre_len;

    // ----------- Build control points ----------
    // p0: start
    vec3f_wgsl p0 = vec3f_wgsl(0,0,0);

    // p1: end of first straight
    vec3f_wgsl p1 = p0 + base_dir * pre_len;

    // Compute arc center
    // Arc plane normal is perpendicular to base_dir and bend_dir
    vec3f_wgsl normal = base_dir.cross(bend_dir).normalized();
    vec3f_wgsl start_tangent = base_dir;
    vec3f_wgsl start_normal  = normal.cross(start_tangent).normalized();

    // The arc center is offset by radius in the negative normal direction
    vec3f_wgsl center = p1 + start_normal * radius;

    // p2: end of arc (start + rotated base_dir by angle)
    vec3f_wgsl p2 =
        center
      + start_tangent * ( radius * cosf(angle) )
      + start_normal  * ( radius * sinf(angle) );

    // p3: final straight
    vec3f_wgsl p3 = p2 + bend_dir * post_len;

    // Build the arc using your existing arc() function
    nurbs<1> arc_curve = arc(radius, angle, center, start_tangent, normal);

    // Now we build a composite curve: line → arc → line
    result.degree[0] = 3;
    result.control.clear();
    result.weight.clear();

    // Add p0 → p1 (straight)
    result.control.push_back(p0);
    result.weight.push_back(1.0f);

    result.control.push_back(p1);
    result.weight.push_back(1.0f);

    // Insert arc control points
    for (size_t i=0; i < arc_curve.control.size(); ++i) {
        result.control.push_back(arc_curve.control[i]);
        result.weight.push_back(arc_curve.weight[i]);
    }

    // Add p2 → p3 (straight)
    result.control.push_back(p2);
    result.weight.push_back(1.0f);

    result.control.push_back(p3);
    result.weight.push_back(1.0f);

    // Knot vector with 3 segments: [straight | arc | straight]
    result.knot[0].clear();
    result.knot[0] = {0,0,0,0,   // degree 3 start
                      0.33,0.33,
                      0.66,0.66,
                      1,1,1,1};

    return result;
}
