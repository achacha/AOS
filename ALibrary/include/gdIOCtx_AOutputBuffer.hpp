#ifndef INCLUDED__gdIOCtx_AOutputBuffer_HPP__
#define INCLUDED__gdIOCtx_AOutputBuffer_HPP__

#include "apiAGdLib.hpp"
#include "gd_io.h"
#include "AOutputBuffer.hpp"

/*!
Output to AOutputBuffer, only output is implemented, input will throw an expection
*/
class AGDLIB_API gdIOCtx_AOutputBuffer
{
public:
  gdIOCtx_AOutputBuffer(AOutputBuffer& file);

  static int  getC(struct gdIOCtx *);
  static int  getBuf(struct gdIOCtx *, void *, int);
  static void putC(struct gdIOCtx *, int);
  static int  putBuf(struct gdIOCtx *, const void *, int);
  static int  seek(struct gdIOCtx *, const int);  // seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek!
  static long tell(struct gdIOCtx *);
  static void gd_free(struct gdIOCtx *);

  /*
  Exposes the AFile* used when constucting this object
  */
  operator gdIOCtx *() { return &m_IOCtx; }
  gdIOCtx *useContextPtr() { return &m_IOCtx; }

private:
  gdIOCtx m_IOCtx;
  AOutputBuffer& m_OutputBuffer;
};

#endif //INCLUDED__gdIOCtx_AOutputBuffer_HPP__
