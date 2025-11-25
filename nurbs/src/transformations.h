#pragma once

#include "nurbs.h"

template <>
int nurbs<3>::bend(
    size_t param_dim,
    float param_value,
    float angle,
    float radius);
