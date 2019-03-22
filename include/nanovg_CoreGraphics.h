#ifndef INCLUDE_NANOVG_CG2D
#define INCLUDE_NANOVG_CG2D

#include "nanovg.h"

NVGcontext *nvgCreateCoreGraphics(int flags);
void nvgDeleteCoreGraphics(NVGcontext *ctx);

#endif
