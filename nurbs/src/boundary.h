
#pragma once

#include "nurbs_include.h"

struct boundary_condition
{
    enum
    {
        DIRICHLET,
        NEUMANN,
    } boundary_condition_type;

    union
    {
        struct
        {
            float value;
        } dirichlet;

        struct
        {
            gsVector3d<float> normal_derivative;
        } neumann;

    } parameter;
};