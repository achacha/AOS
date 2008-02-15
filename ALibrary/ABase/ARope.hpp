#ifndef INCLUDED__ARope_HPP__
#define INCLUDED__ARope_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AOutputBuffer.hpp"

/*!
 ARope - collection of string blocks that is optimized to appending
         will grow by allocating extra blocks to avoid unneccessary movement of data

 All calls accepting ARope as a parameter will never clear it first, instead always append
*/

class AFile;
class ARope;

typedef std::list<ARope>   LIST_ARope;
typedef std::vector<ARope> VECTOR_ARope;

/*!
Output buffer class optimized for appending to end
*/
class ABASE_API ARope : public AOutputBuffer
{
public:
  ARope(size_t blockSize = ARope::DEFAULT_BLOCK_SIZE);
  ARope(const AString&, size_t blockSize = ARope::DEFAULT_BLOCK_SIZE);
  ARope(const ARope&);
  virtual ~ARope();
  
  /*!
  Clear the rope and optionally release memory
  For speed the blocks are kept around and reused
  */
  virtual void clear(bool bReleaseMemory = false);
  
  /*!
  Operators to help with inline appending
  */
  ARope& operator +(const AEmittable&);
  ARope& operator +(const char *);
  ARope& operator +(char);
  ARope& operator +=(const AEmittable&);
  ARope& operator +=(const char *);
  ARope& operator +=(char);
  
  /*!
  Properties
  */
  virtual bool isEmpty() const;
  size_t getSize() const;
  
  /*!
  AEmittable
  */
  void emit(AOutputBuffer&) const;

  /*!
  Create one string to contain them all
  */
  AString toAString() const;

  /*!
  AFile based read/write
  Reads bytes from AFile object and appends to self, returns bytes read
  Length of AConstant::npos means read to EOF
  Writes contents of self to AFile object and returns bytes written
  */
  size_t read(AFile&, size_t length = AConstant::npos);
  size_t write(AFile&) const;               

  enum {
    DEFAULT_BLOCK_SIZE=4096
  };

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0) const;

protected:
  /*!
  Append methods for AOutputBuffer
  */
  virtual void _append(const char *, size_t);

private:
  size_t m_LastBlockFree;                    //a_Bytes left in the last block
  char *mp_LastBlock;                        //a_Last line is not added to list until it is full

  typedef std::list<char *> BlockContainer;
  BlockContainer m_Blocks;                   //a_Collection of memory blocks
  BlockContainer m_FreeStore;                //a_Where free blocks are kept if not needed after allocation

  size_t m_BlockSize;                        //a_Size of blocks to use

  void __newBlock();                         //a_Gets a new block either from storage or allocation
};

#endif //INCLUDED__ARope_HPP__
