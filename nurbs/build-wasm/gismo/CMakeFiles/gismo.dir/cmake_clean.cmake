file(REMOVE_RECURSE
  "lib/libgismo.a"
  "lib/libgismo.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C CXX)
  include(CMakeFiles/gismo.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
