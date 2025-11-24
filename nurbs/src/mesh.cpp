#include "nurbs_include.h"

static gsVector3d<double> compute_normal(
    const gsVector3d<double> &a,
    const gsVector3d<double> &b,
    const gsVector3d<double> &c)
{
    auto n = (b - a).cross(c - a);

    double modulus = n.norm();

    return (modulus > 1e-12) ? (n / modulus) : gsVector3d<double>(0.0, 0.0, 0.0);
}

void mesh::append(const mesh &other)
{
    vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
}

void mesh::to_stl(const std::string &filename) const
{
    std::ofstream out(filename, std::ios::binary);

    if (!out)
        return;

    char header[80] = {0x00};
    out.write(header, 80);

    uint32_t tri_count = static_cast<uint32_t>(vertices.size() / 3);

    out.write(reinterpret_cast<char *>(&tri_count), sizeof(uint32_t));

    for (size_t i = 0; i < tri_count; i += 1)
    {
        const auto &a = vertices[3 * i + 0];
        const auto &b = vertices[3 * i + 1];
        const auto &c = vertices[3 * i + 2];

        auto n = compute_normal(a, b, c);

        float nf[3] = {static_cast<float32_t>(n[0]), static_cast<float32_t>(n[1]), static_cast<float32_t>(n[2])};

        out.write(reinterpret_cast<char *>(nf), 3 * sizeof(float32_t));

        float v[3][3] = {
            {
                static_cast<float32_t>(a[0]),
                static_cast<float32_t>(a[1]),
                static_cast<float32_t>(a[2]),
            },
            {
                static_cast<float32_t>(b[0]),
                static_cast<float32_t>(b[1]),
                static_cast<float32_t>(b[2]),
            },
            {
                static_cast<float32_t>(c[0]),
                static_cast<float32_t>(c[1]),
                static_cast<float32_t>(c[2]),
            },
        };

        out.write(reinterpret_cast<char *>(v), 9 * sizeof(float32_t));

        uint16_t attribute = 0;
        out.write(reinterpret_cast<char *>(&attribute), sizeof(uint16_t));
    }
}
