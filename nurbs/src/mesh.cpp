#include "nurbs_include.h"

static gsVector3d<float> compute_normal(
    const gsVector3d<float> &a,
    const gsVector3d<float> &b,
    const gsVector3d<float> &c)
{
    auto n = (b - a).cross(c - a);

    float modulus = n.norm();

    return (modulus > 1e-12) ? (n / modulus) : gsVector3d<float>(0.0, 0.0, 0.0);
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

        float nf[3] = {static_cast<float>(n[0]), static_cast<float>(n[1]), static_cast<float>(n[2])};

        out.write(reinterpret_cast<char *>(nf), 3 * sizeof(float));

        float v[3][3] = {
            {
                static_cast<float>(a[0]),
                static_cast<float>(a[1]),
                static_cast<float>(a[2]),
            },
            {
                static_cast<float>(b[0]),
                static_cast<float>(b[1]),
                static_cast<float>(b[2]),
            },
            {
                static_cast<float>(c[0]),
                static_cast<float>(c[1]),
                static_cast<float>(c[2]),
            },
        };

        out.write(reinterpret_cast<char *>(v), 9 * sizeof(float));

        uint16_t attribute = 0;
        out.write(reinterpret_cast<char *>(&attribute), sizeof(uint16_t));
    }
}
