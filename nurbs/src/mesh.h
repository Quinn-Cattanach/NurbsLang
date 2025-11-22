
#ifndef MESH_H
#define MESH_H

#include "nurbs_lib.h"

typedef struct
{
    size_t n_tri;
    vec3_t *vertices;
    vec3_t *norms;
} mesh_t;

#endif