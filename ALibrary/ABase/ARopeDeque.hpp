#ifndef INCLUDED__ARopeDeque_HPP__
#define INCLUDED__ARopeDeque_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AOutputBuffer.hpp"

/*
 * ARopeDeque - collection of string blocks that is optimized for adding/removing in front/back
 *         will grow by allocating extra blocks to avoid unneccessary movement of data
 *
 * All calls accepting ARopeDeque as a parameter will never clear it first, instead always append/insert
 *
 * Internal storage for "Rope container data store" (as example)
 * _ _ _ R O P E          (front)
 * C O N T A I N E        (conatiner)
 * R   D A T A   S        (container)
 * T O R E _ _ _ _        (back)
 *
**/

class AFile;
class ARope;

/**
 * ARope type class optimized for removal of data from front and back
 */
class ABASE_API ARopeDeque : public AOutputBuffer
{
public:
  ARopeDeque(size_t blockSize = ARopeDeque::DEFAULT_BLOCK_SIZE);
  ARopeDeque(const AString&, size_t blockSize = ARopeDeque::DEFAULT_BLOCK_SIZE);
  ARopeDeque(const ARopeDeque&);
  virtual ~ARopeDeque();
  
  /*!
  Clear this object and move blocks into internals storage until needed later
  Release memory can be used to delete the blocks
  Not releasing memory on clear improves performance at the cost of memory
  */
  virtual void clear(bool bReleaseMemory = false);

  /*!
  Access based on absolute position in the buffer
  */
  size_t find(char c, size_t startPos = 0) const;                       //a_Returns position found at
  size_t find(const AString& strPattern) const;                         //a_Returns position found at
  size_t compare(const AString& strPattern, size_t startPos = 0) const; //a_0 means it is equal, else position before which the inequality occured
  
  /*!
  Removal
  */
  size_t removeFront(size_t bytes);           //a_Remove from front
  size_t removeBack(size_t bytes);            //a_Remove from back

  /*!
  Push
  */
  void pushFront(const ARopeDeque&);
  void pushBack(const ARopeDeque&);
  void pushFront(const AString&);
  void pushBack(const AString&);
  void pushFront(const char *, size_t);
  void pushBack(const char *, size_t);

  /*!
  Pop: removes data, overwrites target
  
  NOTE: target AString is resized to the original content + request bytes
  */
  size_t popBack(AString& target, size_t);
  size_t popFront(AString& target, size_t);
  size_t popBack(char *, size_t);
  size_t popFront(char *, size_t);

  /*!
  Peek: copies data without removing, overwrites target
  
  NOTE: target AString is resized to the original content + request bytes
  */
  size_t peekBack(AString& target, size_t) const;
  size_t peekFront(AString& target, size_t) const;
  size_t peekBack(char *, size_t) const;
  size_t peekFront(char *, size_t) const;

  /*!
  peek/get/remove until one of the characters in the set are found, returns bytes read
  boolRemoveDelimiter   = remove delimiter from the deque if found
  boolDiscardDelimiter  = append delimiter to result if false
  Returns AConstant::npos if not found
  remove discard
    0       X    - delimiter not removed from source thus not discarded
    1       0    - delimiter removed from source and appended to target
    1       1    - delimiter removed from source and discarded
  */
  size_t peekFrontUntilOneOf(AString&, const AString& strSet) const; 
  size_t popFrontUntilOneOf(AString&, const AString& strSet, bool boolRemoveDelimiter = true, bool discardDelimiter = false);
  size_t removeFrontUntilOneOf(const AString& strSet, bool boolRemoveDelimiter = true);

  /*!
  peek/get/remove until pattern is found, returns bytes read
  boolRemovePattern   = remove pattern from the deque if found
  boolDiscardPattern  = append pattern to result if false
  Returns AConstant::npos if not found
  remove discard
    0       X    - pattern not removed from source thus not discarded
    1       0    - pattern removed from source and appended to target
    1       1    - pattern removed from source and discarded
  */
  size_t peekFrontUntil(AString&, const AString& strPattern) const; 
  size_t popFrontUntil(AString&, const AString& strPattern, bool boolRemovePattern = true, bool discardPattern = false); 
  size_t removeFrontUntil(const AString& strPattern, bool boolRemovePattern = true);

  /*!
  peek/get/remove until pattern is found and <maxBytes
  boolRemovePattern   = remove pattern from the deque if found
  boolDiscardPattern  = append pattern to result if false
  Returns AConstant::npos if not found, or bytes read if found, or maxBytes if limit reached
  remove discard
    0       X    - pattern not removed from source thus not discarded
    1       0    - pattern removed from source and appended to target
    1       1    - pattern removed from source and discarded
  */
  size_t peekFrontUntil(AString&, char, size_t maxBytes = AConstant::npos) const; 
  size_t popFrontUntil(AString&, char, bool boolRemovePattern = true, bool discardPattern = false, size_t maxBytes = AConstant::npos); 
  size_t removeFrontUntil(char, bool boolRemovePattern = true, size_t maxBytes = AConstant::npos);

  /*!
  Checks if empty
  */
  virtual bool isEmpty() const;
  
  /*!
  Calculate current size
  */
  size_t getSize() const;

  /*!
  AOutputBuffer
  */
  virtual size_t flush(AFile&);
  
  /*!
  AEmittable
  Emit contents (equivalent to peekAll, for getAll use emitXXX() then clear())
  */
  void emit(AOutputBuffer&) const;

  /*!
  Concatinate all segments into AString, less efficient than emit
  */
  AString toAString() const;

  /*!
  AFile based read/write
  Reads/writes bytes from AFile object and appends to self, returns bytes read
  Length of AConstant::npos means read to EOF
  */
  size_t fromAFile(AFile&, size_t length = AConstant::npos);
  size_t toAFile(AFile&) const;               

  enum {
    DEFAULT_BLOCK_SIZE=2048
  };

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Append methods for AOutputBuffer
  */
  virtual void _append(const char *, size_t);

private:
  size_t m_FrontBlockFree;                       //a_Bytes left in the front block
  size_t m_BackBlockFree;                        //a_Bytes left in the back block
  char *mp_FrontBlock;                       //a_Back line is not added to list until it is full
  char *mp_BackBlock;                        //a_Front line is not added to list until it is full

  typedef std::deque<char *> BlockContainer;
  BlockContainer m_Blocks;                   //a_Collection of memory blocks
  BlockContainer m_FreeStore;                //a_Storage for released blocks

  size_t m_BlockSize;

  inline void _releaseBlock(char *pBlock);   //a_Moved free block into storage

  //a_Push front/back block into m_Blocks and allocate a new one
  void _newFrontBlock();
  void _newBackBlock();

  //a_Pop front/back block from m_Blocks, delete current front/back block and set it to NULL
  void _popFrontBlock();
  void _popBackBlock();

  //a_Extracts all of the data
  void _popAll(char *);
  void _peekAll(char *) const;
};

#endif //INCLUDED__ARopeDeque_HPP__
