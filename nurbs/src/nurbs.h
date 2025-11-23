#pragma once
#include <vector>
#include <array>
#include <gismo/gismo.h>

template <size_t dimension>
struct nurbs
{
    size_t degree[dimension];
    std::vector<double> knot[dimension];
    std::vector<gismo::gsVector3d<double>> control;
    std::vector<double> weight;

    int evaluate(const std::array<double, dimension> &param, gismo::gsVector3d<double> *result) const;

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
