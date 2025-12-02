#include "optimization.h"
#include "construct.h"
#include "fea.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>

#ifdef OPTIMIZER_ENABLED

optimization_result
grid_search_optimize(const std::vector<operation> &construction,
                     const std::vector<bc::n_boundary_condition> &bcs,
                     const material &mat, objective_function_t objective,
                     std::unordered_map<std::string, variable> &variables,
                     int num_samples_per_var = 10) {
  optimization_result result;
  result.success = false;
  result.optimal_objective = std::numeric_limits<float>::max();
  result.num_iterations = 0;

  if (variables.empty()) {
    result.error_message = "No variables to optimize";
    return result;
  }

  // For now, only handle single variable optimization
  if (variables.size() > 1) {
    result.error_message = "Multi-variable optimization not yet implemented";
    return result;
  }

  // Get the single variable
  auto &var_entry = *variables.begin();
  std::string var_name = var_entry.first;
  variable &var = var_entry.second;

  std::cout << "Optimizing variable: " << var_name << std::endl;
  std::cout << "  Range: [" << var.min_feasible << ", " << var.max_feasible
            << "]" << std::endl;
  std::cout << "  Initial: " << var.actual_value << std::endl;
  std::cout << "  Samples: " << num_samples_per_var << std::endl;

  float best_value = var.actual_value;
  float best_objective = std::numeric_limits<float>::max();

  // Grid search
  for (int i = 0; i < num_samples_per_var; ++i) {
    float t = static_cast<float>(i) / (num_samples_per_var - 1);
    float test_value =
        var.min_feasible + t * (var.max_feasible - var.min_feasible);

    // Create variable map with current test value
    std::unordered_map<std::string, float> var_values;
    var_values[var_name] = test_value;

    std::cout << "\n[Iteration " << (i + 1) << "/" << num_samples_per_var << "]"
              << std::endl;
    std::cout << "  " << var_name << " = " << test_value << std::endl;

    // Construct geometry with current variable value
    nurbs<3> geometry;
    try {
      geometry = construct(construction, var_values);
      // --- DEBUG PRINT ---
      std::cout << "  Geometry constructed successfully." << std::endl;
      // geometry.print_debug_info();
      // ----------------------------
    } catch (const std::exception &e) {
      std::cout << "  Construction failed: " << e.what() << std::endl;
      continue;
    }

    // Run FEA
    fea_result fea_res = run_fea(geometry, bcs, mat);

    if (!fea_res.success) {
      std::cout << "  FEA failed: " << fea_res.error_message << std::endl;
      continue;
    }

    // Evaluate objective
    float obj_value = objective(var_values, fea_res);
    result.iteration_history.push_back(obj_value);

    std::cout << "  Max Displacement: " << fea_res.max_displacement << " m"
              << std::endl;
    std::cout << "  Max Stress: " << fea_res.max_stress / 1e6f << " MPa"
              << std::endl;
    std::cout << "  Objective: " << obj_value / 1e6f << " MPa" << std::endl;

    // Update best
    if (obj_value < best_objective) {
      best_objective = obj_value;
      best_value = test_value;
      std::cout << "  ✓ New best!" << std::endl;
    }

    result.num_iterations++;
  }

  // Update the variable's actual_value with the optimal value
  var.actual_value = best_value;

  // Store results
  result.optimal_values[var_name] = best_value;
  result.optimal_objective = best_objective;
  result.success = true;

  return result;
}

// Main optimization function
optimization_result
optimize(const std::vector<operation> &construction,
         const std::vector<bc::n_boundary_condition> &bcs, const material &mat,
         objective_function_t objective,
         std::unordered_map<std::string, variable> &variables,
         int max_iterations) {
  std::cout << "=== Starting Optimization ===" << std::endl;

  optimization_result result = grid_search_optimize(
      construction, bcs, mat, objective, variables, max_iterations);

  if (result.success) {
    std::cout << "\n=== Optimization Complete ===" << std::endl;
    std::cout << "Success: YES" << std::endl;
    std::cout << "Iterations: " << result.num_iterations << std::endl;
    std::cout << "Optimal Objective: " << result.optimal_objective / 1e6f
              << " MPa" << std::endl;
    std::cout << "Optimal Values:" << std::endl;
    for (const auto &var : result.optimal_values) {
      std::cout << "  " << var.first << " = " << var.second << std::endl;
    }
  } else {
    std::cout << "\n=== Optimization Failed ===" << std::endl;
    std::cout << "Error: " << result.error_message << std::endl;
  }

  return result;
}

#endif
