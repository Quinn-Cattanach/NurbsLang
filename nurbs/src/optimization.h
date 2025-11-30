#pragma once
#include "construct.h"
#include "fea.h"
#include "material.h"
#include <vector>
#include <unordered_map>
#include <string>

// Optimization result
struct optimization_result {
    std::unordered_map<std::string, float> optimal_values;
    float optimal_objective;
    int num_iterations;
    bool success;
    std::string error_message;
};

// Run optimization to minimize objective
optimization_result optimize(
    const std::vector<operation>& construction,
    const std::vector<bc::n_boundary_condition>& bcs,
    const material& mat,
    objective_fn_t objective,
    std::unordered_map<std::string, variable>& variables
);