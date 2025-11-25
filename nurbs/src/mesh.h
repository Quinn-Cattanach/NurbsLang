#pragma once
#include <vector>
#include <string>
#include <gismo.h>

using namespace gismo;

struct mesh
{
    std::vector<gsVector3d<float>> vertices;

    void to_stl(const std::string &filename) const;

    void append(const mesh &other);
};
