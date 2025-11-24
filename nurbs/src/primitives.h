
#pragma once
#include "nurbs_include.h"

template <size_t dimension>
nurbs<dimension> Hyperbox(const std::array<double, dimension> &sizes)
{
    nurbs<dimension> h;

    for (size_t d = 0; d < dimension; d += 1)
    {
        h.degree[d] = 1;
        h.knot[d] = {0.0, 0.0, 1.0, 1.0};
    }

    const size_t num_ctrl = 1 << dimension;

    h.control.reserve(num_ctrl);
    h.weight.reserve(num_ctrl);

    for (size_t idx = 0; idx < num_ctrl; idx += 1)
    {
        double coords[3] = {0.0, 0.0, 0.0};

        for (size_t d = 0; d < dimension; d += 1)
            coords[d] = (idx >> d) & 1 ? sizes[d] : 0.0;
        // ^^ this is just for alternating inf/sup

        h.control.emplace_back(coords[0], coords[1], coords[2]);
        h.weight.push_back(1.0);
    }

    return h;
}

nurbs<2> Rectangle(double width, double height);
nurbs<3> Box(double width, double height, double depth);