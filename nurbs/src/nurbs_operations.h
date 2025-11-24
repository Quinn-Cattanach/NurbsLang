
#pragma once
#include "nurbs.h"

template <size_t dimension>
nurbs<dimension + 1> nurbs<dimension>::sweep(const nurbs<1> &path) const
{
    nurbs<dimension + 1> sweep;

    for (size_t d = 0; d < dimension; d += 1)
    {
        sweep.degree[d] = degree[d];
        sweep.knot[d] = knot[d];
    }

    sweep.degree[dimension] = path.degree[0];
    sweep.knot[dimension] = path.knot[0];

    sweep.control.clear();
    sweep.weight.clear();

    for (size_t w = 0; w < path.n_control(0); w += 1)
    {
        gsVector3d<double> path_pt = path.control[w];

        std::array<size_t, dimension> indices{};
        bool done = false;

        while (!done)
        {
            size_t idx = flatten_index(indices);

            sweep.control.push_back(control[idx] + path_pt);

            double wgt = (weight.empty() ? 1.0 : weight[idx]) *
                         (path.weight.empty() ? 1.0 : path.weight[w]);
            sweep.weight.push_back(wgt);

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
    }

    return sweep;
}