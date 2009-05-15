/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AFileIOStream_HPP__
#define INCLUDED__AFileIOStream_HPP__

#include "apiABase.hpp"
#include "AFile.hpp"

#define DEFINE_BASICTYPE_WRITE_OPERATOR(T)\
AFile_IOStream& operator <<(T _type)\
  { AASSERT(this, mp__OStream); write((void*)&_type, sizeof(_type)); return *this; }

#define DEFINE_BASICTYPE_READ_OPERATOR(T)\
AFile_IOStream& operator >>(T _type)\
  { AASSERT(this, mp__IStream); read((void*)&_type,sizeof(_type)); return *this; }

class ABASE_API AFile_IOStream : public AFile
{
public:
	AFile_IOStream(std::ostream* pOstream = &std::cout, std::istream* pIStream = &std::cin);
	AFile_IOStream(std::ostream& Ostream, std::istream& IStream);
	AFile_IOStream(const AFile_IOStream& aSource);

  /*!
  Casting operators for access to actual ostream and istream object pointers
  Example:
    AFile_IOStream iosf;
    ((std::ostream *)iosf)->precision(DBL_DIG);
  */
  operator std::istream*();
  operator std::ostream*();
  std::istream& getIStream();
  std::ostream& getOStream();

  virtual void open() {}
  virtual void flush();
  virtual void rewind();
  virtual void close();
  virtual bool isOpen();

  /*!
  Lookahead buffer used by read/write/peek
  */
  virtual size_t readBlockIntoLookahead();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  virtual size_t _write(const void *, size_t);
	virtual size_t _read(void *, size_t);
  virtual bool _isNotEof();

private:
  std::istream *mp_IStream;
  std::ostream *mp_OStream;
};

//a_Templates for operators with AFile_IOStream, to make the streaming functions work inlined
template<class T>
AFile_IOStream& operator <<(AFile_IOStream& aFile, const T& _type)
  { _type.toAFile(aFile); return aFile; }

template<class T>
AFile_IOStream& operator >>(AFile_IOStream& aFile, T& _type) 
  { _type.fromAFile(aFile); return aFile; }

#endif // INCLUDED__AFileIOStream_HPP__

/**

Notes

- If you assign a stream for input and peek, expect the stream to be read from
since there is no buffer peek in iostreams.  The result of using a stream to
peek ahead without reading will result in a read.  So as far as a stream is
concerned a peek and read are same, as far as user is concerned peek will not
remove (but really store data in an internal buffer).  This was a design decision
that was made in order to be able to support the peek based interface.

*/
