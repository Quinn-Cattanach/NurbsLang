
#pragma once
#include "nurbs.hpp"

template <size_t dimension>
nurbs<dimension> Hyperbox(const std::array<double, dimension> &sizes)
{
    nurbs<dimension> h;
    // TODO:
    return h;
}

nurbs<2> Rectangle(double width, double height)
{
    return Hypercube<2>({width, height});
}

nurbs<3> Box(double width, double height, double depth)
{
    return Hypercube<3>({width, height, depth});
}
