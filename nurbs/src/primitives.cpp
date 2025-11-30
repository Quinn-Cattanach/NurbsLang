#include "primitives.h"

nurbs<2> rectangle(float width, float height)
{
    return hyperbox<2>({width, height});
}

nurbs<3> box(float width, float height, float depth)
{
    return hyperbox<3>({width, height, depth});
}

nurbs<1> line(float length, vec3f_wgsl direction)
{
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

nurbs<1> arc(float radius, float angle, vec3f_wgsl center, vec3f_wgsl start_dir, vec3f_wgsl normal) {
    start_dir = start_dir.normalized();
    normal = normal.normalized();
    vec3f_wgsl binormal = start_dir.cross(normal).normalized();

    int num_segments;
    if (angle <= M_PI / 2.0) {
        num_segments = 1;  // 0-90 degrees: 1 segment
    } else if (angle <= M_PI) {
        num_segments = 2;  // 90-180 degrees: 2 segments
    } else if (angle <= 3.0 * M_PI / 2.0) {
        num_segments = 3;  // 180-270 degrees: 3 segments
    } else {
        num_segments = 4;  // 270-360 degrees: 4 segments
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

        vec3f_wgsl p0 = center + start_dir * (radius * cos(theta0)) + binormal * (radius * sin(theta0));
        vec3f_wgsl tangent0 = start_dir * (-sin(theta0)) + binormal * cos(theta0);
        vec3f_wgsl tangent1 = start_dir * (-sin(theta1)) + binormal * cos(theta1);
        vec3f_wgsl p1 = center + (start_dir * cos(theta_mid) + binormal * sin(theta_mid)) * (radius / w_mid);
        vec3f_wgsl p2 = center + start_dir * (radius * cos(theta1)) + binormal * (radius * sin(theta1));

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
        result.knot[0].push_back(knot_value);  // multiplicity 2
    }
    
    // Ending knots (degree + 1 = 3 knots at 1)
    for (int i = 0; i <= result.degree[0]; i++) {
        result.knot[0].push_back(1.0);
    }
    
    return result;
}
