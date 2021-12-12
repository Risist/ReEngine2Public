#pragma once
// TODO make the file into pch

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <cstdint>
#include <string>
#include <string_view>

// multicast delegate
#include <multicast.hpp>
#include <functional>

#include <SFML/Graphics.hpp>
#include "../../Imgui/imgui.h"
#include "../../Imgui/imgui-SFML.h"
using namespace sf;

#include "HelperMacros.h"

#ifndef M_PI
#define M_PI		3.14159265359f
#endif

#define RE_EDITOR

/// Constant sized types
using int8  = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using float32 = float;
using float64 = double;

