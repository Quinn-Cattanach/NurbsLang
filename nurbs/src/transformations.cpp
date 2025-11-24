
#include "transformations.h"

template <>
int nurbs<3>::bend(
    size_t param_dim,
    double param_value,
    double angle,
    double radius)
{
    if (param_dim > 2)
        return -1;

    // Ok, this will follow the section about bending described in the NURBS book starting on pg 547.

    // Refine the region to be bent:

    // TODO:

    // Bend the region by repositioning control points:

    // TODO:

    // Remove unnecessary knots:

    // TODO:

    return EXIT_SUCCESS;
}
