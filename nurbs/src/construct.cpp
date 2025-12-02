#include "construct.h"
#include "primitives.h"
#include "transformations.h"
#include "nurbs_operations.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

using namespace std;


float resolve_variable(const string& var_name, const unordered_map<string, float>& var_values) {
    try {
        size_t pos; 
        float value = stof(var_name, &pos);
        if (pos == var_name.length()) return value;
    } catch(...) {
    }
    auto it = var_values.find(var_name);
    if (it == var_values.end())
        throw runtime_error("Variable '"  + var_name +"' not found in map.");
    return it->second;
}

// Build straight line primitive 
nurbs<1> build_line(const object& obj, const unordered_map<string, float>& var_values) {
    if (obj.type != object::LINE) {
        throw runtime_error("build_line called on non-Line object");
    }

    float len = resolve_variable(obj.parameters.line.length, var_values);
    float dir_x = resolve_variable(obj.parameters.line.direction_x, var_values);
    float dir_y = resolve_variable(obj.parameters.line.direction_y, var_values);
    float dir_z = resolve_variable(obj.parameters.line.direction_z, var_values);

    // normalize directions 
    vec3f_wgsl direction(dir_x, dir_y, dir_z);
    float norm = direction.length();
    if (norm < 1e-6) {
        throw std::runtime_error("Direction vector has zero length");
    }
    direction = direction.normalized();

    return line(len, direction);
}

// Build bent line primitive 
nurbs<1> build_bent_line(const object& obj, const unordered_map<string, float>& var_values) {
    if (obj.type != object::BENT_LINE) {
        throw std::runtime_error("build_bent_line called on non-BENT_LINE object");
    }

    float length = resolve_variable(obj.parameters.bent_line.length, var_values);
    float bend_origin_u = resolve_variable(obj.parameters.bent_line.bend_origin_u, var_values);
    float radius = resolve_variable(obj.parameters.bent_line.radius, var_values);

    float base_dir_x = resolve_variable(obj.parameters.bent_line.base_dir_x, var_values);
    float base_dir_y = resolve_variable(obj.parameters.bent_line.base_dir_y, var_values);
    float base_dir_z = resolve_variable(obj.parameters.bent_line.base_dir_z, var_values);

    float bend_dir_x = resolve_variable(obj.parameters.bent_line.bend_dir_x, var_values);
    float bend_dir_y = resolve_variable(obj.parameters.bent_line.bend_dir_y, var_values);
    float bend_dir_z = resolve_variable(obj.parameters.bent_line.bend_dir_z, var_values);

    // Create direction vectors 
    vec3f_wgsl base_dir(base_dir_x, base_dir_y, base_dir_z);
    vec3f_wgsl bend_dir(bend_dir_x, bend_dir_y, bend_dir_z);
    
    if (base_dir.length() < 1e-6 || bend_dir.length() < 1e-6) {
        throw std::runtime_error("Direction vectors have zero length");
    }
    
    base_dir = base_dir.normalized();
    bend_dir = bend_dir.normalized();

    return bent_line(length, bend_origin_u, radius, base_dir, bend_dir);
}

// Build rectangle primitive 
nurbs<2> build_rectangle(const object& obj, const unordered_map<string, float>& var_values) {
    if (obj.type != object::RECTANGLE) {
        throw std::runtime_error("build_rectangle called on non-RECTANGLE object");
    }

    float width = resolve_variable(obj.parameters.rectangle.width, var_values);
    float height = resolve_variable(obj.parameters.rectangle.height, var_values);

    return rectangle(width, height);
}

// Build box primitive 
nurbs<3> build_box(const object& obj, const unordered_map<string, float>& var_values) {
    if (obj.type != object::BOX) {
        throw std::runtime_error("build_box called on non-BOX object");
    }

    float width = resolve_variable(obj.parameters.box.width, var_values);
    float height = resolve_variable(obj.parameters.box.height, var_values);
    float depth = resolve_variable(obj.parameters.box.depth, var_values);
    
    return box(width, height, depth);
}

// Build primitives template 
template<>
nurbs<1> build_primitive<1>(const object& obj, const unordered_map<string, float>& var_values) {
    switch (obj.type) {
        case object::LINE:
            return build_line(obj, var_values);
        case object::BENT_LINE:
            return build_bent_line(obj, var_values);
        default:
            throw std::runtime_error("Cannot build 1D NURBS from object type: " + std::to_string(obj.type));
    }
}

template<>
nurbs<2> build_primitive<2>(const object& obj, const std::unordered_map<std::string, float>& var_values) {
    switch (obj.type) {
        case object::RECTANGLE:
            return build_rectangle(obj, var_values);
        default:
            throw std::runtime_error("Cannot build 2D NURBS from object type: " + std::to_string(obj.type));
    }
}

template<>
nurbs<3> build_primitive<3>(const object& obj, const std::unordered_map<std::string, float>& var_values) {
    switch (obj.type) {
        case object::BOX:
            return build_box(obj, var_values);
        default:
            throw std::runtime_error("Cannot build 3D NURBS from object type: " + std::to_string(obj.type));
    }
}

// Construct function 
nurbs<3> construct(const vector<operation>& construction, const unordered_map<string, float>& var_values) {
    if (construction.empty()) {
        throw std::runtime_error("Construction list is empty");
    }

    vector<nurbs<3>> objects;
    objects.reserve(construction.size());

    for (size_t i = 0; i < construction.size(); ++i) {
        const auto& op = construction[i];
        nurbs<3> result;

        switch (op.type) {

            case operation::SWEEP: {
                // Build face (2D) + path (1D)
                nurbs<2> face = build_primitive<2>(op.parameters.sweep.face, var_values);
                nurbs<1> path = build_primitive<1>(op.parameters.sweep.path, var_values);

                // Sweep produces 3D
                result = face.sweep(path);
                break;
            }
        }

        objects.push_back(result);
    }

    // If only one object, return it
    if (objects.size() == 1) {
        return objects[0];
    }

    // Otherwise return the last object (consistent with CAD procedural semantics)
    return objects.back();
}