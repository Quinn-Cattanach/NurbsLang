#include "primitives.h"

nurbs<2> Rectangle(double width, double height)
{
    return Hyperbox<2>({width, height});
}

nurbs<3> Box(double width, double height, double depth)
{
    return Hyperbox<3>({width, height, depth});
}