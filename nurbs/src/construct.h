#pragma once
#include "nurbs_include.h"
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
    enum {
        LINE,
        RECTANGLE,
        BOX,
        BENT_LINE,
    } type;
    union {
        struct {
            std::string length; 
            std::string bend_origin_u;
            std::string radius;
            std::string base_dir_x;
            std::string base_dir_y;
            std::string base_dir_z;
            std::string bend_dir_x;
            std::string bend_dir_y;
            std::string bend_dir_z;
        } bent_line;
        struct {
            std::string length;
            std::string direction_x;
            std::string direction_y;
            std::string direction_z;
        } line;
        struct {
            std::string width;
            std::string height;
        } rectangle;
        struct {
            std::string width;
            std::string height;
            std::string depth;
        } box;
    } parameters;
};

// Operation types
struct operation {
    enum {
        SWEEP,
    } type;
    union {
        struct {
            object face;
            object path;
        } sweep;
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