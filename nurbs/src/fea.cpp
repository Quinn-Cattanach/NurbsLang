#include "fea.h"
#include <cmath>
#include <gismo.h>
#include <limits>

using namespace gismo;

#ifdef OPTIMIZER_ENABLED

// Convert nurbs<3> to G+Smo gsMultiPatch
gsMultiPatch<real_t> nurbs_to_gismo(const nurbs<3> &geom) {
  // Extract NURBS parameters
  int deg_u = geom.degree[0];
  int deg_v = geom.degree[1];
  int deg_w = geom.degree[2];

  // Create knot vectors
  gsKnotVector<real_t> kv_u(geom.knot[0].begin(), geom.knot[0].end(), deg_u);
  gsKnotVector<real_t> kv_v(geom.knot[1].begin(), geom.knot[1].end(), deg_v);
  gsKnotVector<real_t> kv_w(geom.knot[2].begin(), geom.knot[2].end(), deg_w);

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
  gsTensorNurbs<3, real_t> nurbs_geom(basis, coefs, weights);

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
std::vector<float>
calculate_von_mises_stress(gsElasticityAssembler<real_t> &assembler,
                           const gsMatrix<real_t> &displacement_vector,
                           const material &mat) {
  // von Mises stress calculation requires:
  // 1. Strain tensor from displacement gradients
  // 2. Stress tensor from strain using constitutive law
  // 3. von Mises formula: sqrt(0.5 * ((σ1-σ2)² + (σ2-σ3)² + (σ3-σ1)²))

  std::vector<float> von_mises_stress;

  // Get the geometry patch
  gsMultiPatch<real_t> disp_multi;
  assembler.constructSolution(displacement_vector, disp_multi, 0);

  // create a gsCauchyStressFunction (computes Cauchy stress tensor from
  // displacement field)
  gsCauchyStressFunction<real_t> stressFunc(disp_multi, mat.youngs_modulus,
                                            mat.poissons_ratio);

  // choose evaluation points in parameter domain (we evaluate per patch—here
  // only patch 0)
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

  // Evaluate stress function: Expectation is to get a matrix of size (6 x N)
  // for symmetric stress components: order: (sigma_xx, sigma_yy, sigma_zz,
  // sigma_xy, sigma_yz, sigma_zx)
  gsMatrix<real_t> stress_at_pts;
  stressFunc.eval_into(pts, stress_at_pts);

  if (stress_at_pts.cols() != total_samples) {
    // fallback: return zero-field
    von_mises_stress.assign(total_samples, 0.0f);
    return von_mises_stress;
  }

  // compute von Mises from stress components
  von_mises_stress.resize(total_samples);
  for (int c = 0; c < total_samples; ++c) {
    // get components (ensure indexing matches the return format)
    real_t sxx = stress_at_pts(0, c);
    real_t syy = stress_at_pts(1, c);
    real_t szz = stress_at_pts(2, c);
    real_t sxy = stress_at_pts(3, c);
    real_t syz = stress_at_pts(4, c);
    real_t szx = stress_at_pts(5, c);

    // von Mises formula:
    // sigma_v = sqrt( 0.5 * ((sxx - syy)^2 + (syy - szz)^2 + (szz - sxx)^2) +
    // 3*(sxy^2 + syz^2 + szx^2) )
    double part1 =
        0.5 * ((sxx - syy) * (sxx - syy) + (syy - szz) * (syy - szz) +
               (szz - sxx) * (szz - sxx));
    double part2 = 3.0 * (sxy * sxy + syz * syz + szx * szx);
    double vm = std::sqrt(std::max(0.0, part1 + part2));

    von_mises_stress[c] = static_cast<float>(vm);
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
    // Convert geometry to G+Smo format
    gsMultiPatch<real_t> mp = nurbs_to_gismo(geometry);

    // Set up boundary conditions
    gsBoundaryConditions<real_t> bc_gismo;
    apply_boundary_conditions(bcs, mp, bc_gismo);

    // Create elasticity assembler
    gsElasticityAssembler<real_t> assembler(mp, bc_gismo, mat.youngs_modulus,
                                            mat.poissons_ratio);

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
