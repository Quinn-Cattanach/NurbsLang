
#pragma once
#include "mesh.h"
#include "nurbs_include.h"

struct boundary_condition {
  enum {
    DIRICHLET,
    NEUMANN,
  } boundary_condition_type;

  union {
    struct {
      float value;
    } dirichlet;

    struct {
      vec3f_wgsl normal_derivative;
    } neumann;

  } parameter;
};
