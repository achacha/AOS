#ifndef INCLUDED__AFile_AString_HPP__
#define INCLUDED__AFile_AString_HPP__

#include "apiABase.hpp"
#include "AFile.hpp"
#include "APeekable.hpp"
#include "AString.hpp"

/*!
 AFile operations on AString buffer
 All write operations assume clobber and truncation (append at write position and cut the rest)
*/
class ABASE_API AFile_AString : public AFile, public APeekable
{
public:
	/*!
  ctor
  Contains empty string
  */
  AFile_AString();
  
  /*!
  ctor
  AFile from an existing AString, will copy string contents
  */
  AFile_AString(const AString&);
  
  /*!
  ctor
  Initial string buffer size, increments for realloc  (see AString ctors)
  */
  AFile_AString(size_t, u2);
  
  /*!
  copy ctor
  */
  AFile_AString(const AFile_AString&);
  
  /*!
  dtor
  */
  virtual ~AFile_AString() {}

  /*!
  AEmittable - emits the entire data string to target regardless of current position
  */
  void emit(AOutputBuffer& target) const;

  /*!
  Access to read and write positions
  */
  inline void   setReadPos(size_t pos);
	inline size_t getReadPos() const;
  inline void   setWritePos(size_t pos);
	inline size_t getWritePos() const;

  /*!
  Reset positions, does not clear buffer
  */
  inline void reset();
  
  /*!
  Clear string buffer and reset positions
  */
  inline void clear();
	
  /*!
  Direct access to the data string
  */
  inline const AString& getAString() const;
  inline AString& useAString();

  /*!
  These do nothing for this memory file
  */
  virtual bool isOpen() { return true; }
  virtual void open() {}
  virtual void rewind();
  virtual void close() {}
  virtual void flush() {}

  /*!
  APeekable
  */
  virtual size_t peek(AOutputBuffer& target, size_t index = 0, size_t bytes = AConstant::npos) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

  /*!
  AOutputBuffer
  @return Size of the current internal buffer
  */
  virtual size_t getSize() const;
  virtual size_t flush(AFile&);

protected:
  /*!
  AFile guaranteed methods
  */
  virtual size_t _read(void *data, size_t length);
	virtual size_t _write(const void *data, size_t length);
  virtual bool _isNotEof();

private:
	AString m_Data;
  size_t m_ReadPos;
  size_t m_WritePos;
};

#endif // INCLUDED__AFile_AString_HPP__

