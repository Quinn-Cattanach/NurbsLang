#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <array>

// Forward declarations from your existing code
template <size_t dimension> struct nurbs;

// Range type for boundary conditions
typedef struct {
    float min;
    float max;
} range_t;

// Boundary condition structure
namespace bc {
struct n_boundary_condition {
    enum {
        DIRICHLET,
        NEUMANN,
    } type;
    struct {
        range_t u;
        range_t v;
        range_t w;
    } region;
    union {
        struct {
            float fixed_value;
        } dirichlet;
        struct {
            std::array<float, 3> derivative;
        } neumann;
    } parameter;
};
}

// Variable definition
struct variable {
    float min_feasible;
    float max_feasible;
    float actual_value;
};

// Object types (primitives)
struct object {
    enum type_t { LINE, RECTANGLE, BOX, BENT_LINE } type;

    struct bent_line_t {
        std::string length;
        std::string bend_origin_u;
        std::string radius;
        std::string base_dir_x, base_dir_y, base_dir_z;
        std::string bend_dir_x, bend_dir_y, bend_dir_z;
    };

    struct line_t {
        std::string length;
        std::string direction_x, direction_y, direction_z;
    };

    struct rectangle_t {
        std::string width;
        std::string height;
    };

    struct box_t {
        std::string width;
        std::string height;
        std::string depth;
    };

    // Replace union with a variant-like struct
    struct params_t {
        bent_line_t bent_line;
        line_t      line;
        rectangle_t rectangle;
        box_t       box;
    } parameters;
};

// Operation types
struct operation {
    enum type_t { SWEEP } type;

    struct sweep_t {
        object face;
        object path;
    };

    struct params_t {
        sweep_t sweep;
    } parameters;
};

// Objective function types
typedef enum {
    OBJECTIVE_MIN_MAX_STRESS,
} objective_fn_t;

// Main construct function - builds NURBS geometry from operations and variable values
nurbs<3> construct(
    const std::vector<operation>& construction, 
    const std::unordered_map<std::string, float>& variable_values
);

// Helper functions for resolving variables
float resolve_variable(
    const std::string& var_name,
    const std::unordered_map<std::string, float>& variable_values
);

// Build primitives with resolved variables
nurbs<1> build_line(
    const object& obj,
    const std::unordered_map<std::string, float>& variable_values
);

nurbs<1> build_bent_line(
    const object& obj,
    const std::unordered_map<std::string, float>& variable_values
);

nurbs<2> build_rectangle(
    const object& obj,
    const std::unordered_map<std::string, float>& variable_values
);

nurbs<3> build_box(
    const object& obj,
    const std::unordered_map<std::string, float>& variable_values
);

// Dispatch function to build any primitive
template<size_t dim>
nurbs<dim> build_primitive(
    const object& obj,
    const std::unordered_map<std::string, float>& variable_values
);