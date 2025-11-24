#pragma once
#include <vector>
#include <string>
#include <gismo/gismo.h>

using namespace gismo;

struct mesh
{
    std::vector<gsVector3d<double>> vertices;

    void to_stl(const std::string &filename) const;

    void append(const mesh &other);
};
