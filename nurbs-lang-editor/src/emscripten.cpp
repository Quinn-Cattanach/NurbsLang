#include "mesh.h"
#include "nurbs_include.h"
#ifdef __EMSCRIPTEN__
#include <array>
#include <emscripten/bind.h>
#include <emscripten/val.h>
using namespace emscripten;

using Nurbs1Ptr = uintptr_t;
using Nurbs2Ptr = uintptr_t;
using Nurbs3Ptr = uintptr_t;

// Cleanup
void destroyNurbs1(Nurbs1Ptr ptr) { delete reinterpret_cast<nurbs<1> *>(ptr); }
void destroyNurbs2(Nurbs2Ptr ptr) { delete reinterpret_cast<nurbs<2> *>(ptr); }
void destroyNurbs3(Nurbs3Ptr ptr) { delete reinterpret_cast<nurbs<3> *>(ptr); }

// Primitives
Nurbs1Ptr Line(float length, float x, float y, float z) {
  auto dir = vec3f_wgsl(x, y, z);
  return reinterpret_cast<Nurbs1Ptr>(new nurbs<1>(line(length, dir)));
}

Nurbs2Ptr Rectangle(float width, float height) {
  return reinterpret_cast<Nurbs2Ptr>(new nurbs<2>(rectangle(width, height)));
}

Nurbs3Ptr Box(float width, float height, float depth) {
  return reinterpret_cast<Nurbs3Ptr>(new nurbs<3>(box(width, height, depth)));
}

Nurbs1Ptr BentLine(float length, float bend_origin_u, float radius,
                   float base_x, float base_y, float base_z, float bend_x,
                   float bend_y, float bend_z) {
  vec3f_wgsl base_dir(base_x, base_y, base_z);
  vec3f_wgsl bend_dir(bend_x, bend_y, bend_z);
  return reinterpret_cast<Nurbs1Ptr>(new nurbs<1>(
      bent_line(length, bend_origin_u, radius, base_dir, bend_dir)));
}

// Sweep
Nurbs3Ptr sweep2(Nurbs2Ptr tool, Nurbs1Ptr path) {
  auto t = reinterpret_cast<nurbs<2> *>(tool);
  auto p = reinterpret_cast<nurbs<1> *>(path);
  auto result = new nurbs<3>(t->sweep(*p));
  delete t;
  delete p;
  return reinterpret_cast<Nurbs3Ptr>(result);
}
struct MeshRaw {
  uintptr_t data;
  size_t length;
};

nurbs<3> *call_js_with_map(const char *fnName,
                           const std::map<std::string, float> &m) {
  std::string json = "{";
  bool first = true;
  for (auto &kv : m) {
    if (!first)
      json += ",";
    json += "\"" + kv.first + "\":" + std::to_string(kv.second);
    first = false;
  }
  json += "}";

  // clang-format off
  return reinterpret_cast<nurbs<3> *>(EM_ASM_INT(
      {
        const fnName = UTF8ToString($0);
        const jsonStr = UTF8ToString($1);
        const obj = JSON.parse(jsonStr);

        const fn = self[fnName] || (Module && Module[fnName]);
        if (typeof fn !== "function") {
          console.error("Function not found:", fnName);
          return -1;
        }
        return fn(obj);
      },
      fnName, json.c_str()));
  // clang-format on
}

extern "C" char *
map_to_json(const std::unordered_map<std::string, float> *mapPtr) {
  const auto &m = *mapPtr;

  std::string json;
  json.reserve(m.size() * 16); // pre-allocate for efficiency
  json += "{";

  bool first = true;
  for (const auto &kv : m) {
    if (!first)
      json += ",";
    first = false;

    // "key":value
    json += "\"";
    json += kv.first;
    json += "\":";
    json += std::to_string(kv.second);
  }

  json += "}";

  // Allocate memory in WASM heap and copy JSON into it
  char *out = (char *)malloc(json.size() + 1);
  std::memcpy(out, json.c_str(), json.size() + 1);

  return out; // JS must free
}

#ifdef OPTIMIZER_ENABLED

optimization_result
grid_search_optimize_js(const std::string construction,
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

  if (variables.size() > 1) {
    result.error_message = "Multi-variable optimization not yet implemented";
    return result;
  }

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
    nurbs<3> *geometry;
    try {
      geometry = call_js_with_map(construction.c_str(), );

      std::cout << "  Geometry constructed successfully." << std::endl;
      geometry.print_debug_info();
    } catch (const std::exception &e) {
      std::cout << "  Construction failed: " << e.what() << std::endl;
      continue;
    }

    // Run FEA
    fea_result fea_res = run_fea(*geometry, bcs, mat);

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

  var.actual_value = best_value;

  result.optimal_values[var_name] = best_value;
  result.optimal_objective = best_objective;
  result.success = true;

  return result;
}

optimization_result optimize_js(
    std::string component, const std::vector<bc::n_boundary_condition> &bcs,
    const material &mat, objective_function_t objective,
    std::unordered_map<std::string, variable> &variables, int max_iterations) {
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

MeshRaw toMesh3Raw(Nurbs3Ptr volumePtr, emscripten::val lodsJs) {
  std::array<size_t, 3> lods;
  lods[0] = lodsJs[0].as<size_t>();
  lods[1] = lodsJs[1].as<size_t>();
  lods[2] = lodsJs[2].as<size_t>();

  auto vol = reinterpret_cast<nurbs<3> *>(volumePtr);

  mesh *m = vol->to_mesh(lods);

  MeshRaw out;
  out.data = reinterpret_cast<uintptr_t>(m->vertices.data());
  out.length = m->vertices.size();

  return out;
}

void to_stl(Nurbs3Ptr volume, std::string filename) {
  auto vol = reinterpret_cast<nurbs<3> *>(volumePtr);
  vol->to_stl({20, 20, 80}, filename);
}

// Bindings
EMSCRIPTEN_BINDINGS(nurbs_module) {
  function("destroyNurbs1", &destroyNurbs1);
  function("destroyNurbs2", &destroyNurbs2);
  function("destroyNurbs3", &destroyNurbs3);

  function("Line", &Line);
  function("Rectangle", &Rectangle);
  function("Box", &Box);
  function("BentLine", &BentLine);
  function("sweep2", &sweep2);

  value_object<MeshRaw>("MeshRaw")
      .field("data", &MeshRaw::data)
      .field("length", &MeshRaw::length);

  function("toMesh3Raw", &toMesh3Raw);
}
#endif
