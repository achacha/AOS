#ifndef INCLUDED__AFile_AString_HPP__
#define INCLUDED__AFile_AString_HPP__

#include "apiABase.hpp"
#include "AFile.hpp"
#include "AString.hpp"

/*
 AFile operations on AString buffer
 All write operations assume clobber and truncation (append at write position and cut the rest)
*/
class ABASE_API AFile_AString : public AFile
{
public:
	AFile_AString();
  AFile_AString(const AString&);
  AFile_AString(size_t, u2);                   //a_Initial string buffer size, increments for realloc  (see AString ctors)
  AFile_AString(const AFile_AString&);
  virtual ~AFile_AString() {}

	/*!
  Access to read and write positions
  */
  inline void   setReadPos(size_t pos);
	inline size_t getReadPos() const;
  inline void   setWritePos(size_t pos);
	inline size_t getWritePos() const;

  /*!
  Position and cuffer control
  */
  inline void reset();  //a_Reset positions
  inline void clear();  //a_Clears the contents and resets
	
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
  virtual void flush() {}
  virtual void close() {}

protected:
  /*
    * AFile guaranteed methods
  **/
  virtual size_t _read(void *data, size_t length);
	virtual size_t _write(const void *data, size_t length);
  virtual bool _isNotEof();

private:
	AString m_Data;
  size_t m_ReadPos;
  size_t m_WritePos;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AFile_AString_HPP__

