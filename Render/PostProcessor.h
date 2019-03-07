#pragma once

#include "Scene.h"

class PostProcessor {
public:
  static void Bloom(Color **pixel_buffer, const int xres, const int yres, const double stddev,
    const double dampening, const double scale_factor);
};
