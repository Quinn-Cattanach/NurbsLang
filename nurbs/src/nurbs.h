#pragma once
#include "mesh.h"

template <size_t dimension>
struct nurbs
{
    std::array<size_t, dimension> degree;
    std::array<std::vector<float>, dimension> knot;
    std::vector<vec3f_wgsl> control;
    std::vector<float> weight;

    int evaluate(const std::array<float, dimension> &param, vec3f_wgsl *result) const;

    int bend(
        size_t param_dim,
        float param_value,
        float angle,
        float radius);

    nurbs<dimension + 1> sweep(const nurbs<1> &path) const;

    std::optional<mesh> tessellate(
        size_t lod_u, size_t lod_v,
        size_t index_u, size_t index_v,
        std::array<float, dimension> defaults) const;

    void to_stl(const std::array<size_t, dimension> &lods, std::string filename) const;

    size_t n_control(size_t index) const
    {
        if (index >= dimension)
            return 0;
        return knot[index].size() - degree[index] - 1;
    }

private:
    size_t flatten_index(const std::array<size_t, dimension> &indices) const
    {
        size_t idx = 0;
        size_t stride = 1;
        for (size_t d = 0; d < dimension; d += 1)
        {
            idx += indices[d] * stride;
            stride *= knot[d].size() - degree[d] - 1;
        }
        return idx;
    }

    float basis(size_t dim, size_t index, float x, size_t deg) const;
};
