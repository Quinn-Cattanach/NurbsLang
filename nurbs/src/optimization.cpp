#include "optimization.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>

// Evaluate objective function for given variable values
float evaluate_objective(
    const std::vector<operation>& construction,
    const std::vector<bc::n_boundary_condition>& bcs,
    const material& mat,
    objective_fn_t objective,
    const std::unordered_map<std::string, float>& var_values
) {
    // 1. Build geometry with current variable values
    nurbs<3> geometry = construct(construction, var_values);
    
    // 2. Run FEA
    fea_result result = run_fea(geometry, bcs, mat);
    
    if (!result.success) {
        std::cerr << "FEA failed: " << result.error_message << std::endl;
        return std::numeric_limits<float>::max();  // Return large penalty
    }
    
    // 3. Return objective value based on type
    switch (objective) {
        case OBJECTIVE_MIN_MAX_STRESS:
            return result.max_stress;
        
        default:
            return result.max_stress;
    }
}

// Simple grid search optimization (exhaustive but works)
optimization_result optimize_grid_search(
    const std::vector<operation>& construction,
    const std::vector<bc::n_boundary_condition>& bcs,
    const material& mat,
    objective_fn_t objective,
    std::unordered_map<std::string, variable>& variables,
    int n_samples_per_var = 5
) {
    optimization_result opt_result;
    opt_result.success = false;
    opt_result.num_iterations = 0;
    opt_result.optimal_objective = std::numeric_limits<float>::max();
    
    // Get list of variable names
    std::vector<std::string> var_names;
    for (const auto& pair : variables) {
        var_names.push_back(pair.first);
    }
    
    if (var_names.empty()) {
        opt_result.error_message = "No variables to optimize";
        return opt_result;
    }
    
    // Total combinations
    int total_combinations = std::pow(n_samples_per_var, var_names.size());
    std::cout << "Grid search: " << total_combinations << " combinations\n";
    
    // Try all combinations
    for (int combo = 0; combo < total_combinations; ++combo) {
        // Generate variable values for this combination
        std::unordered_map<std::string, float> var_values;
        
        int temp = combo;
        for (const auto& var_name : var_names) {
            int sample_idx = temp % n_samples_per_var;
            temp /= n_samples_per_var;
            
            const variable& var = variables[var_name];
            float range = var.max_feasible - var.min_feasible;
            float value = var.min_feasible + (sample_idx / float(n_samples_per_var - 1)) * range;
            
            var_values[var_name] = value;
        }
        
        // Evaluate objective
        float obj_value = evaluate_objective(construction, bcs, mat, objective, var_values);
        
        opt_result.num_iterations++;
        
        // Print progress
        std::cout << "Iteration " << opt_result.num_iterations << ": ";
        for (const auto& pair : var_values) {
            std::cout << pair.first << "=" << pair.second << " ";
        }
        std::cout << "-> obj=" << obj_value << std::endl;
        
        // Update best
        if (obj_value < opt_result.optimal_objective) {
            opt_result.optimal_objective = obj_value;
            opt_result.optimal_values = var_values;
        }
    }
    
    opt_result.success = true;
    return opt_result;
}

// Main optimization function (choose algorithm based on problem size)
optimization_result optimize(
    const std::vector<operation>& construction,
    const std::vector<bc::n_boundary_condition>& bcs,
    const material& mat,
    objective_fn_t objective,
    std::unordered_map<std::string, variable>& variables
) {
    size_t n_vars = variables.size();
    
    std::cout << "Starting optimization with " << n_vars << " variables\n";
    
    if (n_vars == 0) {
        optimization_result result;
        result.success = false;
        result.error_message = "No variables to optimize";
        return result;
    }
    
    // For 1-2 variables: use grid search (exhaustive, reliable)
    if (n_vars <= 2) {
        std::cout << "Using grid search (exhaustive)\n";
        return optimize_grid_search(construction, bcs, mat, objective, variables, 10);
    }
    
    // TODO: For 3+ variables: use gradient free or more efficient optimiztion 
}