
#pragma once

#include "nurbs.hpp"

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
            Eigen::Vector3d normal_derivative;
        } neumann;

    } parameter;
};