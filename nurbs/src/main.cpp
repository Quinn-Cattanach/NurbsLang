// #include "nurbs_include.h"
// #include <cmath>
// #include <iostream>

// #ifndef __EMSCRIPTEN__
// int main(int argc, char *argv[]) {
//   nurbs<2> rect = rectangle(5, 1);

//   for (int i = 0; i < 10; i += 1) {
//     nurbs<1> path =
//         bent_line(20, 0.0, (float)i, vec3f_wgsl(1, 0, 0), vec3f_wgsl(1, 1, 0));
//     nurbs<3> result1 = rect.sweep(path);

//     std::string filename = "sample_volume_r" + std::to_string(i) + ".stl";
//     result1.to_stl({20, 20, 100}, filename);
//   }

//   return 0;
// }
// #else
// int main() {
//   std::cout << "The nurbs library was successfully loaded.\n";
//   return 0;
// }
// #endif

#include "optimization.h"
#include "construct.h"
#include "fea.h"
#include "nurbs.h"
#include "mesh.h"
#include <iostream>
#include <cmath>
#include <stdexcept>

using namespace std;
using namespace bc;
using namespace std;
using namespace bc;


float objective_min_max_stress(
    const std::unordered_map<std::string, float>& var_values, 
    const fea_result& fea_res
) {
    return fea_res.max_stress; 
}


void run_optimization_test() {
    cout << "\n=== L-Bracket Optimization Test (Minimizing Max Stress) ===" << endl;

    // --- 1. Define Design Variable (Corner Radius) ---
    unordered_map<string, variable> variables;
    variables["CORNER_RADIUS"] = {
        .min_feasible = 0.05f,   // Minimum radius: 5 cm
        .max_feasible = 0.20f,   // Maximum radius: 20 cm
        .actual_value = 0.10f    // Initial guess: 10 cm
    };
    
    // --- 2. Define Geometric Construction (L-Bracket: Sweep Rectangle along Bent Path) ---
    vector<operation> construction_ops;
    operation sweep_op = {
        .type = operation::SWEEP,
        .parameters = {
            .sweep = {
                // Define Face (Rectangle in 2D)
                .face = {
                    .type = object::RECTANGLE,
                    .parameters = {.rectangle = {
                        .width = "0.05",
                        .height = "0.05"
                    }}
                },
                // Define Path (Line in 1D)
                .path = {
                    .type = object::BENT_LINE,
                    .parameters = {.bent_line = {
                        .length = "1.0",
                        .bend_origin_u = "0.5",  // Bend occurs halfway
                        .radius = "CORNER_RADIUS", // <--- THE VARIABLE
                        .base_dir_x = "1.0",
                        .base_dir_y = "0.0",
                        .base_dir_z = "0.0",
                        .bend_dir_x = "0.0",
                        .bend_dir_y = "1.0",
                        .bend_dir_z = "0.0"
                    }}
                }
            }
        }
    };
    construction_ops.push_back(sweep_op);

    // --- 3. Define Material Properties ---
    material steel_mat = {
        .youngs_modulus_Pa = 200e9f, // 200 GPa
        .poissons_ratio = 0.3f
    };

    // --- 4. Define Boundary Conditions (Fix one arm, load the other) ---
    vector<n_boundary_condition> bcs;
    
    // Constraint (Fixed at one end: u=0 face, corresponding to the start of the path)
    n_boundary_condition fixed_bc;
    fixed_bc.type = n_boundary_condition::DIRICHLET;
    fixed_bc.region.u = {0.0f, 0.0f}; 
    fixed_bc.region.v = {0.0f, 1.0f}; 
    fixed_bc.region.w = {0.0f, 1.0f}; 
    bcs.push_back(fixed_bc);

    // Load (Traction at the other end: u=1 face, corresponding to the end of the L)
    n_boundary_condition load_bc;
    load_bc.type = n_boundary_condition::NEUMANN;
    load_bc.region.u = {1.0f, 1.0f}; 
    load_bc.region.v = {0.0f, 1.0f};
    load_bc.region.w = {0.0f, 1.0f};
    load_bc.parameter.neumann.derivative = {0.0f, -50000.0f, 0.0f}; // 50 kPa load downward
    bcs.push_back(load_bc);

    // --- 5. Run Optimization ---
    cout << "Running optimization (Grid Search with 10 samples)..." << endl;
    
    // Bind the objective function
    objective_function_t objective_fn = objective_min_max_stress;

    optimization_result opt_result = optimize(
        construction_ops,
        bcs,
        steel_mat,
        objective_fn,
        variables,
        10 // max_iterations (used as num_samples_per_var in grid_search_optimize)
    );

    // --- 6. Output Final Results ---
    cout << "\n\n--- Final Optimization Results ---" << endl;
    if (opt_result.success) {
        cout << "Optimization Status: SUCCESS" << endl;
        cout << "Total Iterations (Samples): " << opt_result.num_iterations << endl;
        cout << "Optimal Max Stress: " << opt_result.optimal_objective / 1e6f << " MPa" << endl;
        for (const auto& var : opt_result.optimal_values) {
            cout << "Optimal " << var.first << ": " << var.second << " m" << endl;
        }
        cout << "\nExpected Outcome: Optimal radius should be closer to " 
             << variables["CORNER_RADIUS"].max_feasible << " m (larger radius reduces stress)." << endl;
    } else {
        cout << "Optimization Status: FAILED" << endl;
        cout << "Error: " << opt_result.error_message << endl;
    }
    cout << "------------------------------------------------" << endl;
}

int main() {
#ifdef OPTIMIZER_ENABLED
    run_optimization_test();
#else
    std::cout << "Error: OPTIMIZER_ENABLED is not defined. Optimization functionality is disabled." << std::endl;
#endif
    return 0;
}