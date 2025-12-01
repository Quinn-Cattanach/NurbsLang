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
  uintptr_t vertices;
  uintptr_t parametric_coords;
  uintptr_t normals;
  size_t length;
};

MeshRaw toMesh3Raw(Nurbs3Ptr volumePtr, emscripten::val lodsJs) {
  std::array<size_t, 3> lods;
  lods[0] = lodsJs[0].as<size_t>();
  lods[1] = lodsJs[1].as<size_t>();
  lods[2] = lodsJs[2].as<size_t>();

  auto vol = reinterpret_cast<nurbs<3> *>(volumePtr);

  mesh *m = vol->to_mesh(lods);

  MeshRaw out;
  out.vertices = reinterpret_cast<uintptr_t>(m->vertices.data());
  out.parametric_coords =
      reinterpret_cast<uintptr_t>(m->parametricCoordinates.data());
  out.normals = reinterpret_cast<uintptr_t>(m->normals.data());
  out.length = m->vertices.size();

  return out;
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
      .field("vertices", &MeshRaw::vertices)
      .field("parametric_coords", &MeshRaw::parametric_coords)
      .field("normals", &MeshRaw::normals)
      .field("length", &MeshRaw::length);

  function("toMesh3Raw", &toMesh3Raw);
}
#endif
