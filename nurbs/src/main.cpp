#include "nurbs_include.h"
#include <cmath>
#include <iostream>

int main(int argc, char *argv[])
{
    nurbs<2> curved_surface;

    curved_surface.degree[0] = 2;
    curved_surface.degree[1] = 2;

    curved_surface.knot[0] = {0, 0, 0, 1, 1, 1};
    curved_surface.knot[1] = {0, 0, 0, 1, 1, 1};

    curved_surface.control = {
        gsVector3d<float>(0, 0, 0),
        gsVector3d<float>(5, 0, 5),
        gsVector3d<float>(10, 0, 0),

        gsVector3d<float>(0, 5, -5),
        gsVector3d<float>(5, 5, 0),
        gsVector3d<float>(10, 5, 8),

        gsVector3d<float>(0, 10, 0),
        gsVector3d<float>(5, 10, -5),
        gsVector3d<float>(10, 10, 0),
    };

    curved_surface.weight = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    nurbs<1> extrude_path;
    extrude_path.degree[0] = 2;
    extrude_path.knot[0] = {0, 0, 0, 1, 1, 1};

    extrude_path.control = {
        gsVector3d<float>(0, 0, 0),
        gsVector3d<float>(10, 0, 10),
        gsVector3d<float>(0, 0, 20),
    };

    extrude_path.weight = {1.0, 1.0, 1.0};

    nurbs<3> result = curved_surface.sweep(extrude_path);

    nurbs<4> floatit = result.sweep(extrude_path);

    floatit.to_stl({20, 20, 20, 20}, "sample_volume.stl");

    std::cout << "Hello\n";

    return 0;
}