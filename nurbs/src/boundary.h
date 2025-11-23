
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
            double value;
        } dirichlet;

        struct
        {
            gsVector3d<double> normal_derivative;
        } neumann;

    } parameter;
};