#include "fea.h"
#include <cmath>
#include <gismo.h>
#include <limits>
#include <gsElasticity/src/gsElasticityAssembler.h>
#include <gsElasticity/src/gsElasticityFunctions.h>
#include <gsElasticity/src/gsLinearMaterial.h>
#include <gsElasticity/src/gsMaterialContainer.h>

using namespace gismo;

#ifdef OPTIMIZER_ENABLED

// Convert nurbs<3> to G+Smo gsMultiPatch
gsMultiPatch<real_t> nurbs_to_gismo(const nurbs<3> &geom) {
  // Extract NURBS parameters
  int deg_u = geom.degree[0];
  int deg_v = geom.degree[1];
  int deg_w = geom.degree[2];

  // Create knot vectors
  gsKnotVector<real_t> kv_u(deg_u, geom.knot[0].begin(), geom.knot[0].end());
  gsKnotVector<real_t> kv_v(deg_v, geom.knot[1].begin(), geom.knot[1].end());
  gsKnotVector<real_t> kv_w(deg_w, geom.knot[2].begin(), geom.knot[2].end());

  // Create tensor basis
  gsTensorBSplineBasis<3, real_t> basis(kv_u, kv_v, kv_w);

  // Get number of control points
  size_t n_u = geom.n_control(0);
  size_t n_v = geom.n_control(1);
  size_t n_w = geom.n_control(2);
  size_t total_cp = n_u * n_v * n_w;

  // Create control point matrix (3 rows for x,y,z; columns for each control
  // point)
  gsMatrix<real_t> coefs(total_cp, 3);

  // Copy control points
  for (size_t i = 0; i < total_cp; ++i) {
    coefs(i, 0) = geom.control[i].x;
    coefs(i, 1) = geom.control[i].y;
    coefs(i, 2) = geom.control[i].z;
  }

  // Create weights matrix
  gsMatrix<real_t> weights(total_cp, 1);
  for (size_t i = 0; i < total_cp; ++i) {
    weights(i, 0) = geom.weight[i];
  }

  // Create NURBS geometry (rational B-spline)
  // gsTensorNurbs<3, real_t> nurbs_geom(basis, coefs, weights);
  gsTensorNurbs<3, real_t> nurbs_geom(
    kv_u, kv_v, kv_w,
    coefs, weights
);

  // Create multipatch with single patch
  gsMultiPatch<real_t> mp;
  mp.addPatch(nurbs_geom);

  return mp;
}

// Apply boundary conditions to G+Smo format
void apply_boundary_conditions(const std::vector<bc::n_boundary_condition> &bcs,
                               const gsMultiPatch<real_t> &mp,
                               gsBoundaryConditions<real_t> &bc_gismo) {
  for (const auto &bc : bcs) {
    // Determine which boundary side this applies to
    // Based on the region parameters (u, v, w ranges)

    boxSide side;
    bool is_u_fixed = (fabs(bc.region.u.min - bc.region.u.max) < 1e-6);
    bool is_v_fixed = (fabs(bc.region.v.min - bc.region.v.max) < 1e-6);
    bool is_w_fixed = (fabs(bc.region.w.min - bc.region.w.max) < 1e-6);

    // Determine which face (assumes one parameter is fixed)
    if (is_u_fixed) {
      side = (bc.region.u.min < 0.5) ? boundary::west : boundary::east;
    } else if (is_v_fixed) {
      side = (bc.region.v.min < 0.5) ? boundary::south : boundary::north;
    } else if (is_w_fixed) {
      side = (bc.region.w.min < 0.5) ? boundary::front : boundary::back;
    } else {
      continue;
    }

    if (bc.type == bc::n_boundary_condition::DIRICHLET) {
      // Add Dirichlet BC for all components (0, 1, 2 = x, y, z)
      for (int component = 0; component < 3; ++component) {
        auto zero_func = std::make_shared<gsConstantFunction<real_t>>(0.0, 3);
        bc_gismo.addCondition(0, side, condition_type::dirichlet, zero_func,
                              component);
      }
    } else if (bc.type == bc::n_boundary_condition::NEUMANN) {
      // Neumann BC represents traction (force per unit area)
      for (int component = 0; component < 3; ++component) {
        float force_component = bc.parameter.neumann.derivative[component];
        if (std::fabs(force_component) > 1e-12f) {
          auto force_func =
              std::make_shared<gsConstantFunction<real_t>>(force_component, 3);
          bc_gismo.addCondition(0, side, condition_type::neumann, force_func,
                                component);
        }
      }
    }
  }
}

// Calculate von Mises stress from displacement field
std::vector<float> calculate_von_mises_stress(gsElasticityAssembler<real_t> &assembler,
                                              const gsMatrix<real_t> &displacement_vector,
                                              const material &mat) {
  std::vector<float> von_mises_stress;

  try {
    // Construct the displacement field as a multipatch
    gsMultiPatch<real_t> disp_multi;
\
    assembler.constructSolution(displacement_vector, assembler.allFixedDofs(), disp_multi);

    // Get von Mises stress directly (for 3D problems)
    gsPiecewiseFunction<real_t> stresses;
    assembler.constructCauchyStresses(disp_multi, stresses, 
                                      stress_components::von_mises);

    // Create evaluation points in parameter domain (uniformly distributed)
    int n_samples = 10;
    int total_samples = n_samples * n_samples * n_samples;
    gsMatrix<real_t> pts(3, total_samples);
    
    int idx = 0;
    for (int i = 0; i < n_samples; ++i) {
      for (int j = 0; j < n_samples; ++j) {
        for (int k = 0; k < n_samples; ++k) {
          pts(0, idx) = static_cast<real_t>(i) / (n_samples - 1);
          pts(1, idx) = static_cast<real_t>(j) / (n_samples - 1);
          pts(2, idx) = static_cast<real_t>(k) / (n_samples - 1);
          idx++;
        }
      }
    }

    // Evaluate von Mises stress at all points
    // Result should be 1 x total_samples (von Mises is a scalar)
    gsMatrix<real_t> stress_values = stresses.piece(0).eval(pts);

    if (stress_values.cols() != total_samples) {
      von_mises_stress.assign(total_samples, 0.0f);
      return von_mises_stress;
    }

    // Convert to float vector - von Mises is already computed, just extract it
    von_mises_stress.resize(total_samples);
    for (int c = 0; c < total_samples; ++c) {
      von_mises_stress[c] = static_cast<float>(stress_values(0, c));
    }
  } catch (const std::exception &e) {
    // If anything fails, return zeros
    int total = 10 * 10 * 10;
    von_mises_stress.assign(total, 0.0f);
  }

  return von_mises_stress;
}

// Find maximum stress in field
float find_max_stress(const std::vector<float> &stress_field) {
  if (stress_field.empty())
    return 0.0f;

  float max_stress = stress_field[0];
  for (float stress : stress_field) {
    if (stress > max_stress) {
      max_stress = stress;
    }
  }
  return max_stress;
}

// Find maximum displacement magnitude
float find_max_displacement(const std::vector<vec3f_wgsl> &displacement_field) {
  if (displacement_field.empty())
    return 0.0f;

  float max_disp = 0.0f;
  for (const auto &disp : displacement_field) {
    float magnitude = disp.length();
    if (magnitude > max_disp) {
      max_disp = magnitude;
    }
  }
  return max_disp;
}

// Main FEA function
fea_result run_fea(const nurbs<3> &geometry,
                   const std::vector<bc::n_boundary_condition> &bcs,
                   const material &mat) {
  fea_result result;
  result.success = false;
  try {    
    // Validate geometry first
    std::cout << "DEBUG: Validating geometry..." << std::endl;
    std::cout << "  Control points: " << geometry.control.size() << std::endl;
    std::cout << "  Weights: " << geometry.weight.size() << std::endl;
    std::cout << "  Degrees: [" << geometry.degree[0] << ", " 
              << geometry.degree[1] << ", " << geometry.degree[2] << "]" << std::endl;
    std::cout << "  Knot vectors: [" << geometry.knot[0].size() << ", " 
              << geometry.knot[1].size() << ", " << geometry.knot[2].size() << "]" << std::endl;
    
    // Check for valid NURBS structure
    if (geometry.control.empty()) {
      result.error_message = "Geometry has no control points";
      return result;
    }
    
    if (geometry.degree[0] == 0 || geometry.degree[1] == 0 || geometry.degree[2] == 0) {
      result.error_message = "Geometry has invalid degrees";
      return result;
    }
    
    if (geometry.knot[0].empty() || geometry.knot[1].empty() || geometry.knot[2].empty()) {
      result.error_message = "Geometry has empty knot vectors";
      return result;
    }
    
    // Convert geometry to G+Smo format
    gsMultiPatch<real_t> mp = nurbs_to_gismo(geometry);

    // Set up boundary conditions
    gsBoundaryConditions<real_t> bc_gismo;
    apply_boundary_conditions(bcs, mp, bc_gismo);

    // Create material
    gsLinearMaterial<real_t> linear_material(
        static_cast<real_t>(mat.youngs_modulus_Pa),
        static_cast<real_t>(mat.poissons_ratio),
        3
    );
    // Create material container
    gsMaterialContainer<real_t> materials;
    materials.add(&linear_material);

    // Create a zero body force function (no gravity/body forces)
    gsConstantFunction<real_t> body_force(0.0, 0.0, 0.0, 3);

    // Create basis from multipatch
    gsMultiBasis<real_t> basis(mp);

    // Create elasticity assembler
    gsElasticityAssembler<real_t> assembler(mp, basis, bc_gismo, body_force, materials);

    // Assemble the system
    assembler.assemble();

    // Get system matrices
    gsSparseMatrix<real_t> K = assembler.matrix(); // Stiffness matrix
    gsMatrix<real_t> F = assembler.rhs();          // Force vector

    // Solve the system K * u = F
    gsSparseSolver<real_t>::CGDiagonal solver;
    solver.compute(K);

    if (solver.info() != gsEigen::Success) {
      result.error_message = "Failed to factorize stiffness matrix";
      return result;
    }

    gsMatrix<real_t> displacement = solver.solve(F);

    if (solver.info() != gsEigen::Success) {
      result.error_message = "Failed to solve linear system";
      return result;
    }

    // Extract displacement field
    size_t n_dofs = displacement.rows();
    size_t n_points = n_dofs / 3;
    result.displacement_field.clear();
    result.displacement_field.reserve(n_points);
    for (size_t i = 0; i < n_points; ++i) {
      vec3f_wgsl disp(displacement(3 * i + 0), displacement(3 * i + 1),
                      displacement(3 * i + 2));
      result.displacement_field.push_back(disp);
    }

    // Calculate stress field
    result.stress_field =
        calculate_von_mises_stress(assembler, displacement, mat);
    // Find maximum values
    result.max_stress = find_max_stress(result.stress_field);
    result.max_displacement = find_max_displacement(result.displacement_field);

    result.success = true;

  } catch (const std::exception &e) {
    result.error_message = std::string("FEA error: ") + e.what();
    result.success = false;
  }

  return result;
}

#endif
