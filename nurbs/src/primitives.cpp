#include "primitives.h"

nurbs<2> rectangle(float width, float height)
{
    return hyperbox<2>({width, height});
}

nurbs<3> box(float width, float height, float depth)
{
    return hyperbox<3>({width, height, depth});
}

nurbs<1> line(float length, vec3f_wgsl direction)
{
    nurbs<1> result;

    const float nrm = direction.norm();
    if (nrm != 0.0)
        direction /= nrm;

    result.degree[0] = 1;
    result.knot[0] = {0.0, 0.0, 1.0, 1.0};

    result.control.reserve(2);
    result.weight.reserve(2);

    result.control.emplace_back(0.0, 0.0, 0.0);
    result.weight.emplace_back(1.0);

    vec3f_wgsl end = length * direction;
    result.control.emplace_back(end.x, end.y, end.z);
    result.weight.emplace_back(1.0);

    return result;
}