/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAGdLib.hpp"
#include "AGdCanvas.hpp"
#include "gdIOCtx_AFile.hpp"
#include "gdIOCtx_AOutputBuffer.hpp"

void AGdCanvas::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AGdCanvas @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_GdImagePtr=" << (void *)m_GdImagePtr << std::endl;      //TODO:
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AGdCanvas::AGdCanvas(int sx, int sy)
{
  m_GdImagePtr = gdImageCreateTrueColor(sx, sy);
}

AGdCanvas::~AGdCanvas()
{
  gdFree(m_GdImagePtr);
}

gdImagePtr& AGdCanvas::useGdImagePtr()
{
  return m_GdImagePtr;
}

void AGdCanvas::writePNG(AFile& file, int level) const
{
  gdIOCtx_AFile ctx(file);
  gdImagePngCtxEx(m_GdImagePtr, ctx, level);
}

void AGdCanvas::writeJPEG(AFile& file, int quality) const
{
  gdIOCtx_AFile ctx(file);
  gdImageJpegCtx(m_GdImagePtr, ctx, quality);
}

void AGdCanvas::writeGIF(AFile& file) const
{
  gdIOCtx_AFile ctx(file);
  gdImageGifCtx(m_GdImagePtr, ctx);
}

void AGdCanvas::emitPNG(AOutputBuffer& target, int level) const
{
  gdIOCtx_AOutputBuffer ctx(target);
  gdImagePngCtxEx(m_GdImagePtr, ctx, level);
}

void AGdCanvas::emitJPEG(AOutputBuffer& target, int quality) const
{
  gdIOCtx_AOutputBuffer ctx(target);
  gdImageJpegCtx(m_GdImagePtr, ctx, quality);
}

void AGdCanvas::emitGIF(AOutputBuffer& target) const
{
  gdIOCtx_AOutputBuffer ctx(target);
  gdImageGifCtx(m_GdImagePtr, ctx);
}

void AGdCanvas::emit(AOutputBuffer& target) const
{
  emitPNG(target);
}

void AGdCanvas::setPixel(int x, int y, int color)
{
  AASSERT(this, x >= 0 && x < m_GdImagePtr->sx);
  AASSERT(this, y >= 0 && y < m_GdImagePtr->sy);
  gdImageSetPixel(m_GdImagePtr, x, y, color);
}

int AGdCanvas::getPixel(int x, int y) const
{
  AASSERT(this, x >= 0 && x < m_GdImagePtr->sx);
  AASSERT(this, y >= 0 && y < m_GdImagePtr->sy);
  return gdImageGetTrueColorPixel(m_GdImagePtr, x, y);
}

void AGdCanvas::drawLine(int x0, int y0, int x1, int y1, int color)
{
  AASSERT(this, x0 >= 0 && x0 < m_GdImagePtr->sx);
  AASSERT(this, y0 >= 0 && y0 < m_GdImagePtr->sy);
  AASSERT(this, x1 >= 0 && x1 < m_GdImagePtr->sx);
  AASSERT(this, y1 >= 0 && y1 < m_GdImagePtr->sy);

  gdImageLine(m_GdImagePtr, x0, y0, x1, y1, color);
}

