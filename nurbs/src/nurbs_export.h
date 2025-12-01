
#pragma once
#include "mesh.h"
#include "nurbs.h"

template <size_t dimension>
mesh *
nurbs<dimension>::to_mesh(const std::array<size_t, dimension> &lods) const {
  mesh *result = new mesh;
  std::array<float, dimension> defaults = {0x00};

  for (size_t idx1 = 0; idx1 < dimension; idx1 += 1) {
    for (size_t idx2 = idx1 + 1; idx2 < dimension; idx2 += 1) {
      std::array<bool, dimension> is_varying = {false};
      is_varying[idx1] = true;
      is_varying[idx2] = true;

      std::array<size_t, dimension> fixed_indices;
      size_t num_fixed = 0;
      for (size_t i = 0; i < dimension; i += 1) {
        if (!is_varying[i]) {
          fixed_indices[num_fixed] = i;
          num_fixed += 1;
        }
      }

      size_t num_combinations = 1 << num_fixed;

      for (size_t combo = 0; combo < num_combinations; combo += 1) {
        for (size_t f = 0; f < num_fixed; f += 1) {
          size_t fixed_dim = fixed_indices[f];
          float boundary = (combo & (1 << f)) ? 1.0 : 0.0;
          defaults[fixed_dim] = boundary;
        }

        auto m = tessellate(lods[idx1], lods[idx2], idx1, idx2, defaults);
        if (m.has_value()) {
          mesh face_mesh = m.value();

          result->append(face_mesh);
        }
      }

      for (size_t f = 0; f < num_fixed; f += 1) {
        defaults[fixed_indices[f]] = 0.0;
      }
    }
  }
  return result;
}

template <size_t dimension>
void nurbs<dimension>::to_stl(const std::array<size_t, dimension> &lods,
                              std::string filename) const {
  mesh *result = to_mesh(lods);
  result->to_stl(filename);
  delete result;
}

static inline bool in_bounds(int x, int y, int sx, int sy) {
  return (x >= 0 && y >= 0 && x < sx && y < sy);
}

static inline void write_vertex(int tri_x, int tri_y, int offset,
                                size_t n_tri_x, size_t n_tri_y,
                                const vec3f_wgsl &parametric_coord,
                                const vec3f_wgsl &v, mesh &result) {
  if (in_bounds(tri_x, tri_y, n_tri_x, n_tri_y)) {
    uint32_t idx = 3u * uint32_t(tri_y * n_tri_x + tri_x);
    result.vertices[idx + offset] = v;
    result.parametricCoordinates[idx + offset] = parametric_coord;
  }
}

template <size_t dimension>
std::optional<mesh>
nurbs<dimension>::tessellate(size_t lod_u, size_t lod_v, size_t index_u,
                             size_t index_v,
                             std::array<float, dimension> defaults) const {
  if (lod_u < 2 || lod_v < 2)
    return std::nullopt;
  if (index_u >= dimension || index_v >= dimension)
    return std::nullopt;

  mesh result;
  int n_tri_x = int(2 * (lod_u - 1));
  int n_tri_y = int(lod_v - 1);
  size_t total_vertices = 3 * size_t(n_tri_x * n_tri_y);
  result.vertices.resize(total_vertices);
  result.parametricCoordinates.resize(total_vertices);

  for (float u_index = 0.0; u_index < lod_u; u_index += 1.0) {
    for (float v_index = 0.0; v_index < lod_v; v_index += 1.0) {
      std::array<float, dimension> param;

      for (size_t d = 0; d < dimension; d += 1)
        param[d] = defaults[d];

      param[index_u] = u_index / float(lod_u - 1);
      param[index_v] = v_index / float(lod_v - 1);

      vec3f_wgsl evaluation;
      this->evaluate(param, &evaluation);

      // Create parametric coordinate for THIS grid point
      vec3f_wgsl parametric_coord = vec3f_wgsl(param[0], param[1], param[2]);

      int tri_x = int(u_index) * 2;
      int tri_y = int(v_index);

      write_vertex(tri_x, tri_y, 0, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);
      tri_x -= 1;
      write_vertex(tri_x, tri_y, 0, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);
      tri_x -= 1;
      write_vertex(tri_x, tri_y, 2, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);

      tri_y -= 1;
      tri_x += 3;
      write_vertex(tri_x, tri_y, 1, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);
      tri_x -= 1;
      write_vertex(tri_x, tri_y, 1, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);
      tri_x -= 1;
      write_vertex(tri_x, tri_y, 2, n_tri_x, n_tri_y, parametric_coord,
                   evaluation, result);
    }
  }

  result.compute_normals();

  return result;
}
