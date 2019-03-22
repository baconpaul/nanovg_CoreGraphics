#include <Cocoa/Cocoa.h>

extern CGContextRef getCurrentContextRef();

CGContextRef getCurrentContextRef() {
  CGContextRef cgCtx = [[NSGraphicsContext currentContext] CGContext];
  return cgCtx;
}
