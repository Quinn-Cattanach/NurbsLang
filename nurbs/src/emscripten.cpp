#include "nurbs_include.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten/bind.h>
    
    EMSCRIPTEN_BINDINGS(nurbs_module) {
        emscripten::function("main", &main);
    }
#endif
