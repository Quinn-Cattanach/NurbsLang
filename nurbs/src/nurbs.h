#pragma once
#include "mesh.h"

template <size_t dimension>
struct nurbs
{
    std::array<size_t, dimension> degree;
    std::array<std::vector<double>, dimension> knot;
    std::vector<gsVector3d<double>> control;
    std::vector<double> weight;

    int evaluate(const std::array<double, dimension> &param, gsVector3d<double> *result) const;

    int bend(
        size_t param_dim,
        double param_value,
        double angle,
        double radius);

    nurbs<dimension + 1> sweep(const nurbs<1> &path) const;

    std::optional<mesh> tessellate(
        size_t lod_u, size_t lod_v,
        size_t index_u, size_t index_v,
        std::array<double, dimension> defaults) const;

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

    double basis(size_t dim, size_t index, double x, size_t deg) const;
};
