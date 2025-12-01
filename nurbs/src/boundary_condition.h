#pragma once
#include "nurbs_include.h"

typedef struct {
    float min;
    float max;
} range_t;

struct boundary_condition {
    enum {
        DIRICHLET,
        NEUMANN,
    } type;

    struct {
        range_t u;
        range_t v;
        range_t w;
    } region; // assume one of these is min = max = {1, 0} so we are applying to a face. use the others to mask the BoxSide to only apply the boundary condition to it.

    union {
        struct {
            float fixed_value;
        } dirichlet;

        struct {
            std::array<float, 3> derivative;
        } neumann;
    } parameter;
};

struct variable {
    float min_feasible;
    float max_feasible;
    float actual_value; // start at min or something.
}; // these can be inputs to primitive or operation.

struct object {
    enum {
        LINE,
        RECTANGLE,
        BOX,
        BENT_LINE,
    } type;

    union {

        // All of these strings would be like a reference to a variable object. see optimize(3)
        // Or, we could just use pointers.
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
    } parameter;
};

struct operation {
    enum {
        SWEEP,
    } type;

    union {
        struct {
            object face;
            object path;
        } sweep;
    } parameter;
};

typedef enum {
    OBJECTIVE_MIN_MAX_STRESS,
} objective_fn_t;

// this will create the nurbs<3> using the values in 
extern nurbs<3> construct(std::vector<operation> construction, std::unordered_map<std::string, float> variable_values);

std::vector<variable> optimize(
    std::vector<operation> construction, // these have all the variables in it.
    std::vector<boundary_condition> boundary_conditions,
    objective_fn_t objective,
    std::unordered_map<std::string, variable> variables
) {
    // TODO:
}
