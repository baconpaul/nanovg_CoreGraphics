#include "nanovg_CoreGraphics.h"

#include <CoreGraphics/CoreGraphics.h>

#include <iomanip>
#include <iostream>
#include <vector>

extern CGContextRef getCurrentContextRef();

struct CG2DNVGcontext {
    int flags;
    std::vector<CGLayerRef> textures;

    int imageIdToTextureIndex(int id)
    {
        return id - 2000;
    }
    int textureIndexToImageId(int id)
    {
        return id + 2000;
    }
    CGLayerRef textureAt(int id)
    {
        return textures[imageIdToTextureIndex(id)];
    }
};

typedef struct CG2DNVGcontext CG2DNVGcontext;

int cg2dnvg__renderCreate(void *uptr) {
    std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
              << __func__ << std::endl;
    return 1;
}

int cg2dnvg__renderCreateTexture(void *uptr, int type, int w, int h,
                                 int imageFlags, const unsigned char *data) {
    CGSize sz = CGSizeMake(w,h);
    CGContextRef cgCtx = getCurrentContextRef();
    CGContextSaveGState(cgCtx);
    
    CGLayerRef layer = CGLayerCreateWithContext(cgCtx, sz, NULL );
    CG2DNVGcontext *uctx = static_cast<CG2DNVGcontext *>(uptr);

    if( data != NULL )
    {
        CGContextRef lCtx = CGLayerGetContext(layer);
        CGContextSaveGState(lCtx);

        // FIXME _ as of now this is unfreed; we want to free it whtn the CGDataProcider is released in the callback
        unsigned char* ddata = new unsigned char[w*h*4];
        memcpy(ddata,data,w*h*4);

        CGDataProviderRef dp = CGDataProviderCreateWithData(NULL, ddata, w * h * 4, NULL);
        CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
        CGImageRef imageRef = CGImageCreate(w, h, 8, 8 * 4, w * 4,
                                            cs,
                                            kCGBitmapByteOrder32Big,
                                            dp,
                                            NULL,
                                            false,
                                            kCGRenderingIntentDefault);
        CGContextTranslateCTM(lCtx, 0, h);
        CGContextScaleCTM(lCtx, 1.0, -1.0 );

        CGContextDrawImage(lCtx, CGRectMake(0,0,w,h), imageRef);
        
        CGContextFlush(lCtx);
        
        CGColorSpaceRelease(cs);
        CGImageRelease(imageRef);
        CGDataProviderRelease(dp);
                                                            
        CGContextRestoreGState(lCtx);
    }
    else
    {
        std::cout << "No Data. Blanking canvas" << std::endl;
        CGContextRef lCtx = CGLayerGetContext(layer);
        CGContextSaveGState(lCtx);
        CGContextSetRGBFillColor(lCtx,0.0,0.0,0.0,1.0);
        CGContextFillRect(lCtx,CGRectMake(0,0,w,h));
        CGContextFlush(lCtx);
        CGContextRestoreGState(lCtx);
    }
    CGContextRestoreGState(cgCtx);

    CGLayerRetain(layer);
    uctx->textures.push_back(layer);
    
    std::cerr << "Created texture " << std::dec << w << "x" << h << " with index=" << uctx->textureIndexToImageId(uctx->textures.size() - 1)  << std::endl;

    return uctx->textureIndexToImageId(uctx->textures.size() - 1);
}

int cg2dnvg__renderDeleteTexture(void *uptr, int image) {
    CG2DNVGcontext *uctx = static_cast<CG2DNVGcontext *>(uptr);
    CGLayerRelease(uctx->textureAt(image));
    return 0;
}

int cg2dnvg__renderUpdateTexture(void *uptr, int image, int x, int y, int w,
                                 int h, const unsigned char *data) {
    std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec << image << " " << x << " " << y << " " << w << " " << h << " " << (size_t)data << " "
              << __func__ << std::endl;
    
    CG2DNVGcontext *uctx = static_cast<CG2DNVGcontext *>(uptr);
    CGLayerRef layer = uctx->textureAt(image);

    if( data != NULL )
    {
        // FIXME _ as of now this is unfreed; we want to free it whtn the CGDataProcider is released in the callback
        unsigned char* ddata = new unsigned char[w*h*4];
        memcpy(ddata,data,w*h*4);

        CGContextRef lCtx = CGLayerGetContext(layer);
        CGContextSaveGState(lCtx);
        
        CGDataProviderRef dp = CGDataProviderCreateWithData(NULL, ddata, w * h * 4, NULL);
        CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
        CGImageRef imageRef = CGImageCreate(w, h, 8, 8 * 4, w * 4,
                                            cs,
                                            kCGBitmapByteOrder32Big,
                                            dp,
                                            NULL,
                                            false,
                                            kCGRenderingIntentDefault);
        
        CGContextTranslateCTM(lCtx, 0, h);
        CGContextScaleCTM(lCtx, 1.0, -1.0 );

        CGContextDrawImage(lCtx, CGRectMake(x,y,w,h), imageRef);
        CGContextFlush(lCtx);
        
        CGColorSpaceRelease(cs);
        CGDataProviderRelease(dp);
        CGImageRelease(imageRef);
        CGContextRestoreGState(lCtx);
    }

    return 0;
}

int cg2dnvg__renderGetTextureSize(void *uptr, int image, int *w, int *h) {
    CG2DNVGcontext *uctx = static_cast<CG2DNVGcontext *>(uptr);
    CGLayerRef r = uctx->textureAt(image);
    CGSize s = CGLayerGetSize(r);
    *w = s.width;
    *h = s.height;
    return image;
}

void cg2dnvg__renderViewport(void *uptr, float width, float height,
                             float devicePixelRatio) {
    // FIXME!
    // std::cerr << __func__ << " " << width << "x" << height << " @ " << devicePixelRatio << std::endl;
}

void cg2dnvg__renderCancel(void *uptr) {
    std::cerr << "C2G:: " << std::hex << (size_t)uptr << " " << std::dec
              << __func__ << std::endl;
}

void cg2dnvg__renderFlush(void *uptr) {
    CGContextRef cgCtx = getCurrentContextRef();
    CGContextFlush(cgCtx);
}

void cg2dnvg__renderFill(void *uptr, NVGpaint *paint,
                         NVGcompositeOperationState compositeOperation,
                         NVGscissor *scissor, float fringe, const float *bounds,
                         const NVGpath *paths, int npaths) {
    if( scissor->extent[1] > scissor->extent[0])
    {
        std::cerr << "DEAL WITH SCISSOR" << std::endl;
        for( int i=0;i<6;++i)
            std::cerr << "S.x[" << i << "]=" << scissor->xform[i] << " ";
        std::cerr << " S.e=" << scissor->extent[0] << " " << scissor->extent[1];
        std::cerr << std::endl;
    }
    
    bool solidColor = true;
    int cp = 0;
    while (solidColor && cp < 4) {
        solidColor = solidColor &&
            (paint->innerColor.rgba[cp] == paint->outerColor.rgba[cp]);
        cp++;
    }
    
    bool linearGradient = !solidColor && (paint->extent[0] != paint->extent[1]);
    bool radialGradient = !solidColor && (paint->extent[0] == paint->extent[1]);
    bool imageFill = false;
    if( paint->image != 0 )
    {
        solidColor = false;
        imageFill = true;
    }

    
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
    } else if (linearGradient || radialGradient || imageFill) {
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
            } else if (radialGradient) {
                CGContextDrawRadialGradient(cgCtx, grad, start, 0, start, paint->radius,
                                            kCGGradientDrawsBeforeStartLocation |
                                            kCGGradientDrawsAfterEndLocation);
            } else {
                //std::cerr << "PAINT IMAGE " << paint->image << " " << paths[i].nfill << " ";
                for( int i=0; i<4; ++i )
                {
                    //std::cerr << "B[" << i << "]=" << bounds[ i ] << " ";
                }
                //std::cerr << std::endl;
                // CGContextDrawLayerInRect
                CG2DNVGcontext *uctx = static_cast<CG2DNVGcontext *>(uptr);
                CGLayerRef layer = uctx->textureAt(paint->image);
                CGContextDrawLayerInRect(cgCtx, CGRectMake(bounds[0],bounds[1],bounds[2]-bounds[0],bounds[3]-bounds[1]),layer);
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
    CGContextRef cgCtx = getCurrentContextRef();
   
    for( int i=0; i<nverts; i += 3)
    {
        NVGvertex v0 = verts[i];
        NVGvertex v1 = verts[i+1];
        NVGvertex v2 = verts[i+2];
        CGContextBeginPath(cgCtx);
        CGContextMoveToPoint(cgCtx,v0.x,v0.y);
        CGContextAddLineToPoint(cgCtx,v1.x,v1.y);
        CGContextAddLineToPoint(cgCtx,v2.x,v2.y);
        CGContextClosePath(cgCtx);
        CGContextSetRGBFillColor(cgCtx, 1,1,1,1);
        CGContextDrawPath(cgCtx,kCGPathFill);
    }
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
