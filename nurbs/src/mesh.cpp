#include "mesh.h"

static vec3f_wgsl normal(const vec3f_wgsl &a, const vec3f_wgsl &b,
                         const vec3f_wgsl &c) {
  auto n = (b - a).cross(c - a);

  float modulus = n.norm();

  return (modulus > 1e-12) ? (n / modulus) : vec3f_wgsl(0.0, 0.0, 0.0);
}

void mesh::append(const mesh &other) {
  vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
}

void mesh::compute_normals() {
  normals.resize(vertices.size() / 3);
  for (size_t triangle = 0; triangle < vertices.size(); triangle += 3) {
    normals[triangle / 3] = normal(vertices[triangle], vertices[triangle + 1],
                                   vertices[triangle + 2]);
  }
}

void mesh::to_stl(const std::string &filename) const {
  std::ofstream out(filename, std::ios::binary);

  if (!out)
    return;

  char header[80] = {0x00};
  out.write(header, 80);

  uint32_t tri_count = static_cast<uint32_t>(vertices.size() / 3);

  out.write(reinterpret_cast<char *>(&tri_count), sizeof(uint32_t));

  for (size_t i = 0; i < tri_count; i += 1) {
    const vec3f_wgsl &a = vertices[3 * i + 0];
    const vec3f_wgsl &b = vertices[3 * i + 1];
    const vec3f_wgsl &c = vertices[3 * i + 2];

    vec3f_wgsl n = normal(a, b, c);

    float nf[3] = {static_cast<float>(n.x), static_cast<float>(n.y),
                   static_cast<float>(n.z)};

    out.write(reinterpret_cast<char *>(nf), 3 * sizeof(float));

    float v[3][3] = {
        {
            static_cast<float>(a.x),
            static_cast<float>(a.y),
            static_cast<float>(a.z),
        },
        {
            static_cast<float>(b.x),
            static_cast<float>(b.y),
            static_cast<float>(b.z),
        },
        {
            static_cast<float>(c.x),
            static_cast<float>(c.y),
            static_cast<float>(c.z),
        },
    };

    out.write(reinterpret_cast<char *>(v), 9 * sizeof(float));

    uint16_t attribute = 0;
    out.write(reinterpret_cast<char *>(&attribute), sizeof(uint16_t));
  }
}
