#pragma once
#include "nurbs.h"

template <size_t dimension>
double nurbs<dimension>::basis(size_t dim, size_t index, double x, size_t deg) const
{
    if (deg == 0)
        return (knot[dim][index] <= x && x <= knot[dim][index + 1]) ? 1.0 : 0.0;

    double lhs = 0;
    double rhs = 0;

    double denom1 = knot[dim][index + deg] - knot[dim][index];
    if (denom1 != 0)
        lhs = (x - knot[dim][index]) / denom1 * basis(dim, index, x, deg - 1);

    double denom2 = knot[dim][index + deg + 1] - knot[dim][index + 1];
    if (denom2 != 0)
        rhs = (knot[dim][index + deg + 1] - x) / denom2 * basis(dim, index + 1, x, deg - 1);

    return lhs + rhs;
}

template <size_t dimension>
int nurbs<dimension>::evaluate(const std::array<double, dimension> &param, gsVector3d<double> *result) const
{
    if (!result)
        return -1;

    *result = gsVector3d<double>::Zero();
    double denominator = 0.0;

    std::array<size_t, dimension> indices{};
    bool done = false;
    while (!done)
    {
        size_t idx = flatten_index(indices);
        double coeff = weight.empty() ? 1.0 : weight[idx];
        for (size_t d = 0; d < dimension; d += 1)
            coeff *= basis(d, indices[d], param[d], degree[d]);
        *result += coeff * control[idx];
        denominator += coeff;

        for (size_t d = 0; d < dimension; d += 1)
        {
            indices[d] += 1;
            if (indices[d] < knot[d].size() - degree[d] - 1)
                break;
            indices[d] = 0;
            if (d == dimension - 1)
                done = true;
        }
    }

    if (denominator > 0)
        *result /= denominator;

    return 0;
}
