#include "nurbs_include.h"
#include <cmath>
#include <iostream>

#ifndef __EMSCRIPTEN__
int main(int argc, char *argv[]) {
  nurbs<2> rect = rectangle(5, 1);

  for (int i = 0; i < 10; i += 1) {
    nurbs<1> path =
        bent_line(20, 0.0, (float)i, vec3f_wgsl(1, 0, 0), vec3f_wgsl(1, 1, 0));
    nurbs<3> result1 = rect.sweep(path);

    std::string filename = "sample_volume_r" + std::to_string(i) + ".stl";
    result1.to_stl({20, 20, 100}, filename);
  }

  return 0;
}
#else
int main() {
  std::cout << "The nurbs library was successfully loaded.\n";
  return 0;
}
#endif
