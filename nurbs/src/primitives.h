
#pragma once
#include "nurbs_include.h"

template <size_t dimension>
nurbs<dimension> hyperbox(const std::array<float, dimension> &sizes) {
  nurbs<dimension> h;

  for (size_t d = 0; d < dimension; d += 1) {
    h.degree[d] = 1;
    h.knot[d] = {0.0, 0.0, 1.0, 1.0};
  }

  const size_t num_ctrl = 1 << dimension;

  h.control.reserve(num_ctrl);
  h.weight.reserve(num_ctrl);

  for (size_t idx = 0; idx < num_ctrl; idx += 1) {
    float coords[3] = {0.0, 0.0, 0.0};

    for (size_t d = 0; d < dimension; d += 1) {
      d = d % 3;
      coords[d] += (idx >> d) & 1 ? sizes[d] : 0.0;
    }
    // ^^ this is just for alternating inf/sup

    h.control.emplace_back(coords[0], coords[1], coords[2]);
    h.weight.push_back(1.0);
  }

  return h;
}

nurbs<1> line(float length, vec3f_wgsl direction);
nurbs<2> rectangle(float width, float height);
nurbs<3> box(float width, float height, float depth);
nurbs<1> bent_line(float length, float bend_origin_u, float radius,
                   vec3f_wgsl base_dir, vec3f_wgsl bend_dir);
