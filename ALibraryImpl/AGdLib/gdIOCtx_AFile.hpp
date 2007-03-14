#ifndef INCLUDED__gdIOCtx_AFile_HPP__
#define INCLUDED__gdIOCtx_AFile_HPP__

#include "apiAGdLib.hpp"
#include "gd_io.h"
#include "AFile.hpp"

/*!
Given AFile based object will create a context usable for GdLib
*/
class AGDLIB_API gdIOCtx_AFile
{
public:
  gdIOCtx_AFile(AFile& file);

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
  AFile& m_File;
};

#endif //INCLUDED__gdIOCtx_AFile_HPP__