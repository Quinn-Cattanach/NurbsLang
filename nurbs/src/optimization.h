#pragma once
#include "construct.h"
#include "material.h"
#include "fea.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

#ifdef OPTIMIZER_ENABLED

// Optimization result
struct optimization_result {
    std::unordered_map<std::string, float> optimal_values;
    float optimal_objective;
    int num_iterations;
    bool success;
    std::string error_message;
    std::vector<float> iteration_history;
};

// Objective function type: takes variable values and FEA result, returns objective value to minimize
using objective_function_t = std::function<float(const std::unordered_map<std::string, float>&, const fea_result&)>;

// Run optimization to minimize objective
optimization_result optimize(
    const std::vector<operation>& construction,
    const std::vector<bc::n_boundary_condition>& bcs,
    const material& mat,
    objective_function_t objective,
    std::unordered_map<std::string, variable>& variables,
    int max_iterations = 50
);

#endif