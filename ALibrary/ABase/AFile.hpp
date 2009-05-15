/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AFile_HPP__
#define INCLUDED__AFile_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ARopeDeque.hpp"
#include "AException.hpp"
#include "AOutputBuffer.hpp"

class ARope;

//a_Write and read as is
#define DEFINE_BASICTYPE_IO_OPERATORS(T) \
size_t write(const T& _type) \
  { return write(&_type, sizeof(T)); } \
size_t read(T& _type) \
  { return read(&_type, sizeof(T)); } \
size_t peek(T& _type) \
  { return peek(&_type, sizeof(T)); }

//a_Will convert to and from network order
#define DEFINE_NETWORKTYPE_IO_OPERATORS(T) \
size_t write(const T& _type) \
  { return _writeNetworkOrder((u1*)&_type, sizeof(T)); } \
size_t read(T& _type) \
  { return _readNetworkOrder((u1*)&_type, sizeof(T)); } \
size_t peek(T& _type) \
  { return _peekNetworkOrder((u1*)&_type, sizeof(T)); }

/*!
------------------CUT HPP-------------------------
public:
  virtual void open();
  virtual void flush();
  virtual void close();
  virtual bool isOpen();

protected:
	virtual size_t _read(void *, size_t);
	virtual size_t _write(const void *, size_t);
  virtual bool _isNotEof();
------------------CUT HPP-------------------------
*/
class ABASE_API AFile : public AOutputBuffer
{
public:
  AFile();
  virtual ~AFile() {}

  /*!
  Check if not EOF
  Will call _isNotEof member to see if the derived class is EOF and will use buffer to give final result
  */
  virtual bool isNotEof();
  
  /*!
  Check if open
  */
  virtual bool isOpen() = 0;

  /*!
  AString and ARope read/write length specifies # of bytes to read
  
  @return 0 if EOF
  */  
  virtual size_t write(const AString&, size_t length);
  virtual size_t read(AString&, size_t length);
  virtual size_t write(const ARope&);
  virtual size_t read(ARope&, size_t length);

  /*!
  Peek
  @param target output buffer
  @param length if npos then peek everything
  @return number of bytes read, 0 means EOF
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t peek(AString& target, size_t length);

  /*!
  AEmittable required method - reads from current read position and writes to output
  Equivalent to peekUntilEOF
  @param target output buffer
  */
  void emit(AOutputBuffer& target) const;

  /*!
  Read content of this file and write it to the target output buffer

  @param target output buffer
  @param length if npos then write everything
  @return number of bytes read, 0 means EOF
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t read(AOutputBuffer& target, size_t length = AConstant::npos);

  /*!
  Read from a source file and write to this file

  @param source file
  @param length if npos then read everything
  @return number of bytes read
  */
  virtual size_t write(AFile& source, size_t length = AConstant::npos);

  /*!
  Line reads, appends result
  Reads until CRLF or LF are encountered

  @return Number of bytes read and appends to the result
          If maxBytes specified will read <maxBytes, if EOL not found within maxBytes, return value is maxBytes and nothing appended
          If EOF is found before end of line, EOF is treated as EOL if flag i set to true
          If EOF is found and no data read then AConstant::npos is returned (or if flag is false)
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t peekLine(AString& target, size_t maxBytes = AConstant::npos, bool treatEofAsEol = true);
  virtual size_t readLine(AString& target, size_t maxBytes = AConstant::npos, bool treatEofAsEol = true);
  virtual size_t skipLine(size_t maxBytes = AConstant::npos, bool treatEofAsEol = true);

  /*!
  Reads to EOF and returns number of bytes read, appends result, removes data from file
  Allows control of read blocks
  ARope version does NOT clear the rope (rope is to be cleared by the caller, since the object is meant for appends)
  
  @return Number of bytes read, 0 bytes read if already at EOF
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t readUntilEOF(AOutputBuffer&);

  /*!
  Peeks to EOF and returns number of bytes read, appends result, does not remove the data from file, does not advance read positions
  Allows control of read blocks
  ARope version does NOT clear the rope (rope is to be cleared by the caller, since the object is meant for appends)
  
  @return Number of bytes read, 0 bytes read if already at EOF
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t peekUntilEOF(AOutputBuffer&);

  /*!
  Discards all data until EOF and returns number of bytes read

  @return Number of bytes read, 0 bytes read if already at EOF
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t skipUntilEOF();
  
  /*!
  Ability to read/skip until delimeter is found, appends result
  If the delimeter is not found then it will not read to EOF, should use readUntilEOF to get the remainder without delimeter
  
   remove discard
     0       X    - pattern not removed from source thus not discarded
     1       0    - pattern removed from source and appended to target
     1       1    - pattern removed from source and discarded
  
  @return AConstant::npos if EOF or not found, else length is successful
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t readUntilOneOf(AString&, const AString& strDelimeters = AConstant::ASTRING_WHITESPACE, bool boolRemoveDelimiter = true, bool boolDiscardDelimiter = true);
  virtual size_t peekUntilOneOf(AString&, const AString& strDelimeters = AConstant::ASTRING_WHITESPACE);
  virtual size_t skipUntilOneOf(const AString& strDelimeters = AConstant::ASTRING_WHITESPACE, bool boolDiscardDelimeter = true);
  virtual size_t skipUntilOneOf(char, bool boolDiscardDelimeter = true);

  /*!
  Ability to read/skip until a match is found, appends result
  If the delimeter is not found then it will not read to EOF, should use readUntilEOF to get the remainder without delimeter
   remove discard
     0       X    - pattern not removed from source thus not discarded
     1       0    - pattern removed from source and appended to target
     1       1    - pattern removed from source and discarded
  
  @return AConstant::npos if EOF or not found, else length is successful
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t peekUntil(AString&, char cPattern);
  virtual size_t peekUntil(AString&, const AString& strPattern);
  virtual size_t readUntil(AString&, char cPattern, bool boolRemovePattern = true, bool boolDiscardPattern = true);
  virtual size_t readUntil(AString&, const AString& strPattern, bool boolRemovePattern = true, bool boolDiscardPattern = true);
  virtual size_t skipUntil(char cPattern, bool boolDiscardPattern = true);
  virtual size_t skipUntil(const AString& strPattern, bool boolDiscardPattern = true);

  /*!
  Ability to skip over delimeters
  
  @return AConstant::npos if EOF, otherwise number of characters skipped over
          AConstant::unavail if non-blocking and data unavailable but not closed
  */
  virtual size_t skipOver(const AString& strDelimeters = AConstant::ASTRING_WHITESPACE);

  /*!
  Read some data into lookahead buffer
  Useful for moving data from file channel into local buffer
  Used by socket code to detect a closed socket
  This method is used by read/skip/peek routines

  @return bytes read, 0 = nothing, AConstant::npos = error, AConstant::unavail if data not available yet
  */
  virtual size_t readBlockIntoLookahead();
  
  /*!
  Scan file for a pattern (possible to read to EOF if pattern not found)
  
  @return position of the pattern or AConstant::npos if not found
  */
  virtual size_t find(const AString& strPattern);

  /*!
  Will write a line (strLine + AConstant::ASTRING_EOL delimeter)
  
  @return number of bytes written
  */  
  virtual size_t writeLine(const AString& line = AConstant::ASTRING_EMPTY, const AString& strEOL = AConstant::ASTRING_EOL);

  /*!
  Write buffer of given length
  @return AConstant::npos if EOF (which is not a common occurance)
  */
  virtual size_t write(const void *pcvData, size_t length);

  /*!
  Read into a buffer of given length
  @return 0 if EOF, AConstant::unavail if non-blocking and data unavailable
  */
	virtual size_t read(void* pvData, size_t length);
  
  /*!
  Peek data into the buffer of given length
  @return 0 if EOF, AConstant::unavail if non-blocking and data unavailable
  */
  virtual size_t peek(void* pvData, size_t length);
  
  /*!
  Skip over some data
  */
  virtual size_t skip(size_t length);

  /*!
  Puts back the read data into the front of the lookahead buffer for next read to use
  Should be using peek to look ahead, this is only as a last resort
  */
  virtual void putBack(char);
  virtual void putBack(const AEmittable&);

  /*!
  IO control (useful in some file types)
  */
  virtual void open() = 0;          //a_Open
  virtual void flush() = 0;         //a_Signals to flush the contents (if applicable)
  virtual void rewind() = 0;        //a_Rewinds the file to the begining (if applicable)
  virtual void close() = 0;         //a_Close

  /*!
  AOutputBuffer
  @return AConstant::npos by default, derived classes may return actual size if applicable (sockets, iosteams, etc do not have actual sizes for this call)
  */
  virtual size_t getSize() const;

  /*!
  IO operators
  */
  DEFINE_NETWORKTYPE_IO_OPERATORS(int);
  DEFINE_NETWORKTYPE_IO_OPERATORS(unsigned int);
  DEFINE_NETWORKTYPE_IO_OPERATORS(s1);
  DEFINE_NETWORKTYPE_IO_OPERATORS(u1);
  DEFINE_NETWORKTYPE_IO_OPERATORS(s2);
  DEFINE_NETWORKTYPE_IO_OPERATORS(u2);
  DEFINE_NETWORKTYPE_IO_OPERATORS(s4);
  DEFINE_NETWORKTYPE_IO_OPERATORS(u4);
  DEFINE_NETWORKTYPE_IO_OPERATORS(char);
  DEFINE_BASICTYPE_IO_OPERATORS(float);
  DEFINE_BASICTYPE_IO_OPERATORS(double);
  DEFINE_BASICTYPE_IO_OPERATORS(long double);
  DEFINE_BASICTYPE_IO_OPERATORS(s8);
  DEFINE_BASICTYPE_IO_OPERATORS(u8);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Generic write/read/peek routines that will call the specialized _read/_write routines as needed
  */
  size_t _writeNetworkOrder(const u1* pData, size_t uLength);
  size_t _readNetworkOrder(u1* pData, size_t uLength);
  size_t _peekNetworkOrder(u1* pData, size_t uLength);

  /*!
  Specialized routines for network write for 1,2,4,8 byte values
  */
  size_t _writeNetworkOrderU2(const u1* pData);
  size_t _readNetworkOrderU2(u1* pData);
  size_t _peekNetworkOrderU2(u1* pData);
  size_t _writeNetworkOrdersize_t(const u1* pData);
  size_t _readNetworkOrdersize_t(u1* pData);
  size_t _peekNetworkOrdersize_t(u1* pData);
  size_t _writeNetworkOrderU8(const u1* pData);
  size_t _readNetworkOrderU8(u1* pData);
  size_t _peekNetworkOrderU8(u1* pData);

  /*!
  Read and write raw data, implemented by derived classes
  All other methods rely on these 2 function, if they work the whole AFile works
  */
	virtual size_t _read(void *, size_t) = 0;
	virtual size_t _write(const void *, size_t) = 0;
  virtual bool _isNotEof() = 0;

  /*!
  Append methods for AOutputBuffer
  */
  virtual size_t _append(const char *, size_t);

  ARopeDeque m_LookaheadBuffer;

  size_t m_ReadBlock;
  enum { DefaultReadBlock = 32767 };

private:
  AFile(const AFile&);  //a_Prevent copying of file objects due to handle sharing problems
};

#endif // INCLUDED__AFile_HPP__

