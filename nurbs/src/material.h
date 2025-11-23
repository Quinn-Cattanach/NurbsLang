
#pragma once
#include "nurbs_include.h"

struct material
{
    double youngs_modulus_Pa;
    double poisson_ratio;
};

material Aluminum = {
    .youngs_modulus_Pa = 69e9,
    .poisson_ratio = 0.33,
};
