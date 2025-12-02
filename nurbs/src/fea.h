#pragma once

#ifdef OPTIMIZER_ENABLED

#include "construct.h"
#include "material.h"
#include "mesh.h"
#include "nurbs.h"
#include <gsElasticity/src/gsElasticityAssembler.h>
#include <vector>

using namespace gismo;

// FEA result structure
struct fea_result {
  float max_stress;                // Maximum von Mises stress (Pa)
  float max_displacement;          // Maximum displacement magnitude (m)
  std::vector<float> stress_field; // Von Mises stress at each node
  std::vector<vec3f_wgsl>
      displacement_field;    // Displacement vector at each node
  bool success;              // Whether FEA solved successfully
  std::string error_message; // Error message if failed
};

// Main FEA function - analyzes a 3D NURBS solid
fea_result run_fea(const nurbs<3> &geometry,
                   const std::vector<bc::n_boundary_condition> &bcs,
                   const material &mat);

// Helper: Convert nurbs<3> to G+Smo multipatch
gsMultiPatch<real_t> nurbs_to_gismo(const nurbs<3> &geom);

// Helper: Apply boundary conditions to G+Smo boundary conditions object
void apply_boundary_conditions(const std::vector<bc::n_boundary_condition> &bcs,
                               const gsMultiPatch<real_t> &mp,
                               gsBoundaryConditions<real_t> &bc_gismo);

// Helper: Calculate von Mises stress from displacement field
std::vector<float>
calculate_von_mises_stress(gsElasticityAssembler<real_t> &assembler,
                           const gsMatrix<real_t> &displacement_vector,
                           const material &mat);

// Helper: Extract max values from fields
float find_max_stress(const std::vector<float> &stress_field);
float find_max_displacement(const std::vector<vec3f_wgsl> &displacement_field);

#endif
