#include "nanovg_CoreGraphics.h"

#include <CoreGraphics/CoreGraphics.h>

#include <iomanip>
#include <iostream>

extern CGContextRef getCurrentContextRef();

struct CG2DNVGcontext {
  int flags;
};

typedef struct CG2DNVGcontext CG2DNVGcontext;

int cg2dnvg__renderCreate(void *uptr) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
  return 1;
}

int cg2dnvg__renderCreateTexture(void *uptr, int type, int w, int h,
                                 int imageFlags, const unsigned char *data) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
  return 1;
}

int cg2dnvg__renderDeleteTexture(void *uptr, int image) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
  return 0;
}

int cg2dnvg__renderUpdateTexture(void *uptr, int image, int x, int y, int w,
                                 int h, const unsigned char *data) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
  return 0;
}

int cg2dnvg__renderGetTextureSize(void *uptr, int image, int *w, int *h) {
  // std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec <<
  // __func__ << std::endl;
  return 0;
}

void cg2dnvg__renderViewport(void *uptr, float width, float height,
                             float devicePixelRatio) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
}

void cg2dnvg__renderCancel(void *uptr) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
}

void cg2dnvg__renderFlush(void *uptr) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
}

void cg2dnvg__renderFill(void *uptr, NVGpaint *paint,
                         NVGcompositeOperationState compositeOperation,
                         NVGscissor *scissor, float fringe, const float *bounds,
                         const NVGpath *paths, int npaths) {
  bool solidColor = true;
  int cp = 0;
  while (solidColor && cp < 4) {
    solidColor = solidColor &&
                 (paint->innerColor.rgba[cp] == paint->outerColor.rgba[cp]);
    cp++;
  }

  bool linearGradient = !solidColor && (paint->extent[0] != paint->extent[1]);
  bool radialGradient = !solidColor && (paint->extent[0] == paint->extent[1]);

  CGContextRef cgCtx = getCurrentContextRef();

  if (solidColor) {
    for (int i = 0; i < npaths; ++i) {
      CGMutablePathRef pathR = CGPathCreateMutable();
      CGPathMoveToPoint(pathR, NULL, paths[i].fill[0].x, paths[i].fill[0].y);

      for (int j = 1; j < paths[i].nfill; ++j) {
        CGPathAddLineToPoint(pathR, NULL, paths[i].fill[j].x,
                             paths[i].fill[j].y);
      }
      CGPathCloseSubpath(pathR);
      CGContextSetRGBFillColor(cgCtx, paint->innerColor.r, paint->innerColor.g,
                               paint->innerColor.b, paint->innerColor.a);
      CGContextAddPath(cgCtx, pathR);
      CGContextDrawPath(cgCtx, kCGPathFill);
      CGPathRelease(pathR);
    }
  } else if (linearGradient || radialGradient) {
    CGFloat colors[8];
    CGFloat locations[2];
    locations[0] = 0.f;
    locations[1] = 1.f;
    for (int i = 0; i < 4; ++i) {
      colors[i] = paint->innerColor.rgba[i];
      colors[i + 4] = paint->outerColor.rgba[i];
    }

    float sx, sy, ex, ey;
    nvgTransformPoint(&sx, &sy, paint->xform, 0, paint->extent[0]);
    nvgTransformPoint(&ex, &ey, paint->xform, 0,
                      (paint->extent[1] - paint->extent[0]) * 2.0 +
                          paint->extent[0]);
    CGPoint start = CGPointMake(sx, sy);
    CGPoint end = CGPointMake(ex, ey);

    CGGradientRef grad = CGGradientCreateWithColorComponents(
        CGColorSpaceCreateDeviceRGB(), colors, locations, 2);

    for (int i = 0; i < npaths; ++i) {
      CGMutablePathRef pathR = CGPathCreateMutable();
      CGPathMoveToPoint(pathR, NULL, paths[i].fill[0].x, paths[i].fill[0].y);

      for (int j = 1; j < paths[i].nfill; ++j) {
        CGPathAddLineToPoint(pathR, NULL, paths[i].fill[j].x,
                             paths[i].fill[j].y);
      }
      // FIXME: if closed close it
      CGPathCloseSubpath(pathR);
      CGContextAddPath(cgCtx, pathR);

      /*CGContextSetRGBFillColor(cgCtx, paint->innerColor.r,
         paint->innerColor.g, paint->innerColor.b, paint->innerColor.a);
                               CGContextDrawPath(cgCtx, kCGPathFill);*/

      CGContextClip(cgCtx);

      if (linearGradient) {
        CGContextDrawLinearGradient(cgCtx, grad, start, end,
                                    kCGGradientDrawsBeforeStartLocation |
                                        kCGGradientDrawsAfterEndLocation);
      } else {
        CGContextDrawRadialGradient(cgCtx, grad, start, 0, start, paint->radius,
                                    kCGGradientDrawsBeforeStartLocation |
                                        kCGGradientDrawsAfterEndLocation);
      }

      CGContextResetClip(cgCtx);
      CGPathRelease(pathR);
    }
    CGGradientRelease(grad);
  } else {
    std::cerr << "WHAT THE HECK AM I!" << std::endl;
  }
}

void cg2dnvg__renderStroke(void *uptr, NVGpaint *paint,
                           NVGcompositeOperationState compositeOperation,
                           NVGscissor *scissor, float fringe, float strokeWidth,
                           const NVGpath *paths, int npaths) {
  // std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec <<
  // __func__ << std::endl;
  CGContextRef cgCtx = getCurrentContextRef();

  for (int i = 0; i < npaths; ++i) {
    CGMutablePathRef pathR = CGPathCreateMutable();
    CGPathMoveToPoint(pathR, NULL, paths[i].stroke[0].x, paths[i].stroke[0].y);

    for (int j = 1; j < paths[i].nstroke; ++j) {
      CGPathAddLineToPoint(pathR, NULL, paths[i].stroke[j].x,
                           paths[i].stroke[j].y);
    }

    CGContextSetRGBStrokeColor(cgCtx, paint->innerColor.r, paint->innerColor.g,
                               paint->innerColor.b, paint->innerColor.a);
    CGContextAddPath(cgCtx, pathR);
    CGContextDrawPath(cgCtx, kCGPathStroke);
    CGPathRelease(pathR);
  }
}

void cg2dnvg__renderTriangles(void *uptr, NVGpaint *paint,
                              NVGcompositeOperationState compositeOperation,
                              NVGscissor *scissor, const NVGvertex *verts,
                              int nverts) {
  /*  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
              << __func__ << std::endl;
  */
}

void cg2dnvg__renderDelete(void *uptr) {
  std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
            << __func__ << std::endl;
}

NVGcontext *nvgCreateCoreGraphics(int flags) {
  NVGparams params;
  NVGcontext *ctx = NULL;
  CG2DNVGcontext *cg2d = (CG2DNVGcontext *)malloc(sizeof(CG2DNVGcontext));
  if (cg2d == NULL)
    goto error;
  memset(cg2d, 0, sizeof(CG2DNVGcontext));

  memset(&params, 0, sizeof(params));

  params.renderCreate = cg2dnvg__renderCreate;
  params.renderCreateTexture = cg2dnvg__renderCreateTexture;
  params.renderDeleteTexture = cg2dnvg__renderDeleteTexture;
  params.renderUpdateTexture = cg2dnvg__renderUpdateTexture;
  params.renderGetTextureSize = cg2dnvg__renderGetTextureSize;
  params.renderViewport = cg2dnvg__renderViewport;
  params.renderCancel = cg2dnvg__renderCancel;
  params.renderFlush = cg2dnvg__renderFlush;
  params.renderFill = cg2dnvg__renderFill;
  params.renderStroke = cg2dnvg__renderStroke;
  params.renderTriangles = cg2dnvg__renderTriangles;
  params.renderDelete = cg2dnvg__renderDelete;

  params.userPtr = cg2d;
  params.edgeAntiAlias = 1;

  cg2d->flags = flags;

  ctx = nvgCreateInternal(&params);
  std::cerr << "CTX is " << std::hex << (size_t)ctx << std::endl;
  if (ctx == NULL)
    goto error;

  return ctx;

error:
  // 'gl' is freed by nvgDeleteInternal.
  if (ctx != NULL)
    nvgDeleteInternal(ctx);
  return NULL;
}
