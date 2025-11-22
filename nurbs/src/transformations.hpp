#pragma once

#include "nurbs.hpp"

void nurbs<2>::bend(
    size_t param_dim,   // which parametric direction to fold along
    double param_value, // crease start point
    double angle,       // angle
    double radius       // bending radius
);

void nurbs<3>::bend(
    size_t param_dim,
    double param_value,
    double angle,
    double radius);