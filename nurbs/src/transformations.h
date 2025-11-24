#pragma once

#include "nurbs.h"

template <>
int nurbs<3>::bend(
    size_t param_dim,
    double param_value,
    double angle,
    double radius);
