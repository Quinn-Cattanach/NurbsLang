#pragma once

#include <array>
#include <cmath>
#include <fstream>
#include <gismo.h>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

using std::array;
using std::clamp;
using std::vector;

using namespace gismo;

#include "boundary.h"
#include "material.h"
#include "mesh.h"
#include "nurbs.h"
#include "primitives.h"
#include "transformations.h"

#include "nurbs_evaluate.h"
#include "nurbs_export.h"
#include "nurbs_operations.h"
