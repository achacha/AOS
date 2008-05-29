/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AGdCanvas_HPP__
#define INCLUDED__AGdCanvas_HPP__

#include "apiAGdLib.hpp"
#include "ADebugDumpable.hpp"

class AFile;

/*!
Very loose wrapper around the GD library
Provides a way to output the image to AFile-type class
*/
class AGDLIB_API AGdCanvas : public ADebugDumpable
{
public:
  /*!
  Create canvas sx,sy in size
  */
  AGdCanvas(int sx, int sy);
  virtual ~AGdCanvas();

  /*!
  Direct access to the GdLib gdImagePtr struct
  */
  gdImagePtr& useGdImagePtr();

  /*! 
  Some useful primitives, others found in gd.h and usable with useGdImagePtr()
  */
  void setPixel(int x, int y, int color);
  int  getPixel(int x, int y) const;
  void drawLine(int x0, int y0, int x1, int y1, int color);

  /*!
  Write PNG
  level = [0, 9]  0=no cpmpression  9=max compression
  */
  void writePNG(AFile&, int level = 0) const;
  void emitPNG(AOutputBuffer&, int level = 0) const;
  
  /*!
  Write as JPEG
  quality = [0, 100]   0=best compession(low quality)  100=almost no compression(high quality)
  */
  void writeJPEG(AFile&, int quality = 10) const;
  void emitJPEG(AOutputBuffer&, int quality = 10) const;

  /*!
  Write as GIF
  */
  void writeGIF(AFile&) const;
  void emitGIF(AOutputBuffer&) const;

  /*!
  AElement, default will do PNG
  */
  void emit(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AGdCanvas() {}

  gdImagePtr m_GdImagePtr;
};

#endif //INCLUDED__AGdCanvas_HPP__
