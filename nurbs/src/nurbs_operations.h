#pragma once
#include "nurbs.h"
template <size_t dimension>
nurbs<dimension + 1> nurbs<dimension>::sweep(const nurbs<1> &path) const {
  nurbs<dimension + 1> sweep;
  // Copy degrees & knots for original dimensions
  for (size_t d = 0; d < dimension; ++d) {
    sweep.degree[d] = degree[d];
    sweep.knot[d] = knot[d];
  }
  // Add path dimension
  sweep.degree[dimension] = path.degree[0];
  sweep.knot[dimension] = path.knot[0];
  sweep.control.clear();
  sweep.weight.clear();

  // greville absissae
  // https://web.mit.edu/hyperbook/Patrikalakis-Maekawa-Cho/node16.html
  size_t nPath = path.n_control(0);
  for (size_t w = 0; w < nPath; ++w) {
    float u = 0.0f;
    if (nPath > 1) {

      float sum = 0.0f;
      for (size_t i = 1; i <= path.degree[0]; ++i) {
        sum += path.knot[0][w + i];
      }
      u = sum / float(path.degree[0]);
    }

    vec3f_wgsl path_pt = path.control[w];
    vec3f_wgsl T = derivative(path, u).normalized();

    // Stable frame (N, B, T)
    vec3f_wgsl up(0, 0, 1);
    if (fabs(T.dot(up)) > 0.9f)
      up = vec3f_wgsl(0, 1, 0);
    vec3f_wgsl B = T.cross(up).normalized();
    vec3f_wgsl N = B.cross(T).normalized();

    array<size_t, dimension> indices{};
    bool done = false;
    while (!done) {
      size_t idx = flatten_index(indices);
      vec3f_wgsl local = control[idx];
      vec3f_wgsl world = N * local.x + B * local.y + T * local.z + path_pt;
      sweep.control.push_back(world);
      float wgt = (weight.empty() ? 1.0f : weight[idx]) *
                  (path.weight.empty() ? 1.0f : path.weight[w]);
      sweep.weight.push_back(wgt);
      for (size_t d = 0; d < dimension; ++d) {
        indices[d]++;
        if (indices[d] < n_control(d))
          break;
        indices[d] = 0;
        if (d == dimension - 1)
          done = true;
      }
    }
  }
  return sweep;
}
