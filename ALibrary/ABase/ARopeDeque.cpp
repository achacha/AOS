/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ARopeDeque.hpp"
#include "AFile.hpp"
#include "ARope.hpp"

void ARopeDeque::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "getSize()=" << getSize()
    << "  m_BlockSize=" << m_BlockSize
    << "  m_FreeStore.size()=" << m_FreeStore.size()
    << "  m_FrontBlockFree=" << m_FrontBlockFree
    << "  m_BackBlockFree=" << m_BackBlockFree << std::endl;
  
  if (mp_FrontBlock)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_FrontBlock=" << (void*)mp_FrontBlock << " {" << std::endl;
    if (m_BlockSize != m_FrontBlockFree)
      ADebugDumpable::dumpMemory_HexAscii(os, mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_BackBlock=NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Blocks={" << std::endl;
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    ADebugDumpable::dumpMemory_HexAscii(os, (*cit), m_BlockSize, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  if (mp_BackBlock)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_BackBlock=" << (void*)mp_BackBlock << " {" << std::endl;
    if (m_BlockSize != m_BackBlockFree)
      ADebugDumpable::dumpMemory_HexAscii(os, mp_BackBlock, m_BlockSize - m_BackBlockFree, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_BackBlock=NULL" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ARopeDeque::ARopeDeque(size_t blockSize /* = DEFAULT_BLOCK_SIZE */) :
  m_BlockSize(blockSize),
  m_FrontBlockFree(0),
  mp_FrontBlock(NULL),
  m_BackBlockFree(0),
  mp_BackBlock(NULL)
{
}

ARopeDeque::ARopeDeque(const AString& str, size_t blockSize /* = DEFAULT_BLOCK_SIZE */) :
  m_BlockSize(blockSize),
  m_FrontBlockFree(0),
  mp_FrontBlock(NULL),
  m_BackBlockFree(0),
  mp_BackBlock(NULL)
{
  pushBack(str);
}

ARopeDeque::ARopeDeque(const ARopeDeque& that) :
  m_BlockSize(that.m_BlockSize),
  m_FrontBlockFree(that.m_FrontBlockFree),
  m_BackBlockFree(that.m_BackBlockFree)
{
  BlockContainer::const_iterator cit = that.m_Blocks.begin();
  while (cit != that.m_Blocks.end())
  {
    char *p = new char[m_BlockSize];
    memcpy(p, (*cit), m_BlockSize);
    m_Blocks.push_back(p);
    ++cit;
  }

  if (that.mp_FrontBlock)
  {
    mp_FrontBlock = new char[m_BlockSize];
    memcpy(mp_FrontBlock, that.mp_FrontBlock, m_BlockSize - m_FrontBlockFree);
  }
  if (that.mp_BackBlock)
  {
    mp_BackBlock = new char[m_BlockSize];
    memcpy(mp_BackBlock, that.mp_BackBlock, m_BlockSize - m_BackBlockFree);
  }
}

ARopeDeque::~ARopeDeque()
{
  try
  {
    clear(true);
  }
  catch(...) {}   //a_Prevent exceptions in dtor
}

bool ARopeDeque::isEmpty() const
{
  if (
    0 == m_Blocks.size()
    && (!mp_FrontBlock || (mp_FrontBlock && m_FrontBlockFree == m_BlockSize))
    && (!mp_BackBlock || (mp_BackBlock && m_BackBlockFree == m_BlockSize))
  )
    return true;
  else
    return false;
}

size_t ARopeDeque::getSize() const
{
  size_t ret = m_Blocks.size() * m_BlockSize;
  if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
    ret += (m_BlockSize - m_FrontBlockFree);
  if (mp_BackBlock && m_BlockSize > m_BackBlockFree)
    ret += (m_BlockSize - m_BackBlockFree);

  return ret;
}

void ARopeDeque::clear(bool bReleaseMemory /* = false */)
{
  if (bReleaseMemory)
  {
    BlockContainer::iterator it = m_Blocks.begin();
    while (it != m_Blocks.end())
    {
      delete [](*it);
      ++it;
    }
    m_Blocks.clear();

    it = m_FreeStore.begin();
    while(it != m_FreeStore.end())
    {
      delete [](*it);
      ++it;
    }
    m_FreeStore.clear();

    pDeleteArray(mp_FrontBlock);
    m_FrontBlockFree = 0;
    pDeleteArray(mp_BackBlock);
    m_BackBlockFree = 0;
  }
  else
  {
    BlockContainer::iterator it = m_Blocks.begin();
    while (it != m_Blocks.end())
    {
      m_FreeStore.push_back(*it);
      ++it;
    }
    m_Blocks.clear();

    if (mp_FrontBlock)
    {
      m_FreeStore.push_back(mp_FrontBlock);
      mp_FrontBlock = NULL;
    }
    m_FrontBlockFree = 0;
    
    if (mp_BackBlock)
    {
      m_FreeStore.push_back(mp_BackBlock);
      mp_BackBlock = NULL;
    }
    m_BackBlockFree = 0;
  }
}

void ARopeDeque::emit(AOutputBuffer& target) const
{
  //a_Front
  if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
  {
    target.append(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
  }

  //a_Middle
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    target.append((*cit), m_BlockSize);
    ++cit;
  }

  //a_Back
  if (mp_BackBlock && m_BlockSize > m_BackBlockFree)
  {
    target.append(mp_BackBlock, m_BlockSize - m_BackBlockFree); 
  }
}

AString ARopeDeque::toAString() const
{
  AString strRet;
  emit(strRet);
  return strRet;
}

size_t ARopeDeque::fromAFile(AFile& file, size_t length /* = AConstant::npos */)
{
  size_t bytesRead = 0;

  //a_If no back block, allocate a new one
  if (!m_BackBlockFree)
    _newBackBlock();  

  //a_Bytes to read is <= bytes in last block
  AASSERT(this, mp_BackBlock);
  if (length <= size_t(m_BlockSize - m_BackBlockFree))
  {
    bytesRead = file.read(mp_BackBlock + m_BlockSize - m_BackBlockFree, length);
    if (bytesRead == AConstant::npos)
      return 0;  //a_EOF, nothing read

    m_BackBlockFree -= bytesRead;
    return bytesRead;
  }
  
  size_t totalBytesRead = 0;
  while (length > 0)
  {
    size_t toRead = (length > m_BackBlockFree ? m_BackBlockFree : length);
    bytesRead = file.read(mp_BackBlock + m_BlockSize - m_BackBlockFree, toRead);
    if (!bytesRead)
      break;  //a_EOF
    
    m_BackBlockFree -= bytesRead;
    length -= bytesRead;
    totalBytesRead += bytesRead;

    if (!m_BackBlockFree)
      _newBackBlock();  
  }

  return totalBytesRead;
}

size_t ARopeDeque::toAFile(AFile& file) const
{
  size_t bytesWritten = 0;

  if (mp_FrontBlock)
    bytesWritten += file.write(mp_FrontBlock+m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);

  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    bytesWritten += file.write((*cit), m_BlockSize);
    ++cit;
  }

  if (mp_BackBlock)
    bytesWritten += file.write(mp_BackBlock, m_BlockSize - m_BackBlockFree);

  return bytesWritten;
}

size_t ARopeDeque::popBack(AString& strTarget, size_t bytesLeft)
{
  size_t content = strTarget.getSize();
  size_t bytesRead = popBack(strTarget.startUsingCharPtr(bytesLeft + content) + content, bytesLeft);
  strTarget.stopUsingCharPtr(content + bytesRead);
  return bytesRead;
}

size_t ARopeDeque::popFront(AString& strTarget, size_t bytesLeft)
{
  size_t content = strTarget.getSize();
  size_t bytesRead = popFront(strTarget.startUsingCharPtr(bytesLeft + content) + content, bytesLeft);
  strTarget.stopUsingCharPtr(content + bytesRead);
  return bytesRead;
}

size_t ARopeDeque::peekBack(AString& strTarget, size_t bytesLeft) const
{
  size_t content = strTarget.getSize();
  size_t bytesRead = peekBack(strTarget.startUsingCharPtr(bytesLeft + content) + content, bytesLeft);
  strTarget.stopUsingCharPtr(content + bytesRead);
  return bytesRead;
}

size_t ARopeDeque::peekFront(AString& strTarget, size_t bytesLeft) const
{
  size_t content = strTarget.getSize();
  size_t bytesRead = peekFront(strTarget.startUsingCharPtr(bytesLeft + content) + content, bytesLeft);
  strTarget.stopUsingCharPtr(content + bytesRead);
  return bytesRead;
}

void ARopeDeque::pushBack(const AString& str)
{
  pushBack(str.c_str(), str.getSize());
}

void ARopeDeque::pushFront(const AString& str)
{ 
  pushFront(str.c_str(), str.getSize());
}

void ARopeDeque::pushBack(const ARopeDeque& that)
{
  //a_Front
  if (that.mp_FrontBlock && that.m_BlockSize > that.m_FrontBlockFree)
  {
    pushBack(that.mp_FrontBlock + that.m_FrontBlockFree, that.m_BlockSize - that.m_FrontBlockFree);
  }

  //a_Middle
  BlockContainer::const_iterator cit = that.m_Blocks.begin();
  while (cit != that.m_Blocks.end())
  {
    pushBack((*cit), that.m_BlockSize);
    ++cit;
  }

  //a_Back
  if (that.mp_BackBlock && that.m_BlockSize > that.m_BackBlockFree)
  {
    pushBack(that.mp_BackBlock, that.m_BlockSize - that.m_BackBlockFree); 
  }
}

void ARopeDeque::pushFront(const ARopeDeque& that)
{ 
  //a_Back
  if (that.mp_BackBlock && that.m_BlockSize > that.m_BackBlockFree)
  {
    pushFront(that.mp_BackBlock, that.m_BlockSize - that.m_BackBlockFree); 
  }

  //a_Middle
  BlockContainer::const_reverse_iterator cit = that.m_Blocks.rbegin();
  while (cit != that.m_Blocks.rend())
  {
    pushFront((*cit), that.m_BlockSize);
    ++cit;
  }

  //a_Front
  if (that.mp_FrontBlock && that.m_BlockSize > that.m_FrontBlockFree)
  {
    pushFront(that.mp_FrontBlock + that.m_FrontBlockFree, that.m_BlockSize - that.m_FrontBlockFree);
  }
}

size_t ARopeDeque::popBack(char *pcTarget, size_t bytesLeft)
{
  size_t bytesAvailable = getSize();
  if (bytesLeft >= bytesAvailable)
  {
    //a_Requested more data than we have
    _popAll(pcTarget);
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesLeft;
    size_t offset = bytesLeft;
    if (mp_BackBlock && m_BlockSize > m_BackBlockFree)
    {
      size_t bytes = (bytesLeft + m_BackBlockFree > m_BlockSize ? m_BlockSize - m_BackBlockFree : bytesLeft);
      offset -= bytes;
      memcpy(pcTarget + offset, mp_BackBlock + m_BlockSize - m_BackBlockFree - bytes, bytes);
      bytesLeft -= bytes;
      m_BackBlockFree += bytes;
    }

    //a_Loop until less than block left
    while (bytesLeft > m_BlockSize)
    {
      AASSERT(this, m_Blocks.size() > 0);
      char *p = m_Blocks.back();
      offset -= m_BlockSize;
      memcpy(pcTarget + offset, p, m_BlockSize);
      bytesLeft -= m_BlockSize;
      m_Blocks.pop_back();
      _releaseBlock(p);
    }

    //a_Now we have less than a block to go
    if (bytesLeft > 0)
    {
      if (m_Blocks.size() > 0)
      {
        //a_Use back block after poping it
        _popBackBlock();
        offset -= bytesLeft;
        AASSERT(this, mp_BackBlock);
        memcpy(pcTarget + offset, mp_BackBlock + m_BlockSize - bytesLeft, bytesLeft);
        m_BackBlockFree += bytesLeft;
      }
      else
      {
        //a_Use front block
        offset -= bytesLeft;
        AASSERT(this, mp_FrontBlock);
        memcpy(pcTarget + offset, mp_FrontBlock + m_BlockSize - bytesLeft, bytesLeft);

        if (m_FrontBlockFree + bytesLeft < m_BlockSize)
        {
          size_t bytesCopy = m_BlockSize - m_FrontBlockFree - bytesLeft;
          AASSERT(this, mp_FrontBlock);
          for (size_t j = 0; j < bytesCopy ; ++j)
            *(mp_FrontBlock + m_BlockSize - 1 - j) = *(mp_FrontBlock + m_FrontBlockFree + bytesCopy - 1 - j);
        }
        m_FrontBlockFree += bytesLeft;
        AASSERT(this, m_BlockSize >= m_FrontBlockFree);
      }
    }
    return retBytes;
  }
}

size_t ARopeDeque::removeBack(size_t bytesLeft)
{
  size_t bytesAvailable = getSize();
  if (bytesLeft > bytesAvailable)
  {
    //a_Requested more data than we have
    clear();
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesLeft;
    if (m_BlockSize > m_BackBlockFree)
    {
      size_t bytes = (bytesLeft + m_BackBlockFree > m_BlockSize ? m_BlockSize - m_BackBlockFree : bytesLeft);
      bytesLeft -= bytes;
      m_BackBlockFree += bytes;
    }

    //a_Loop until less than block left
    while (bytesLeft > m_BlockSize)
    {
      AASSERT(this, m_Blocks.size() > 0);
      char *p = m_Blocks.back();
      bytesLeft -= m_BlockSize;
      m_Blocks.pop_back();
      _releaseBlock(p);
    }

    //a_Now we have less than a block to go
    if (bytesLeft > 0)
    {
      if (m_Blocks.size() > 0)
      {
        //a_Use back block after poping it
        _popBackBlock();
        m_BackBlockFree += bytesLeft;
      }
      else
      {
        //a_Use front block
        if (m_FrontBlockFree + bytesLeft < m_BlockSize)
        {
          size_t bytesCopy = m_BlockSize - m_FrontBlockFree - bytesLeft;
          for (size_t j = 0; j < bytesCopy ; ++j)
            *(mp_FrontBlock + m_BlockSize - 1 - j) = *(mp_FrontBlock + m_FrontBlockFree + bytesCopy - 1 - j);
        }
        m_FrontBlockFree += bytesLeft;
        AASSERT(this, m_BlockSize >= m_FrontBlockFree);
      }
    }
    return retBytes;
  }
}

size_t ARopeDeque::popFront(char *pcTarget, size_t bytesLeft)
{
  size_t bytesAvailable = getSize();
  if (bytesLeft >= bytesAvailable)
  {
    //a_Requested more data than we have
    _popAll(pcTarget);
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesLeft;
    size_t offset = 0;
    if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
    {
      size_t bytes = (bytesLeft + m_FrontBlockFree > m_BlockSize ? m_BlockSize - m_FrontBlockFree : bytesLeft);
      memcpy(pcTarget + offset, mp_FrontBlock + m_FrontBlockFree, bytes);
      bytesLeft -= bytes;
      offset += bytes;
      m_FrontBlockFree += bytes;
    }

    //a_Loop until less than block left
    while (bytesLeft > m_BlockSize)
    {
      AASSERT(this, m_Blocks.size() > 0);
      char *p = m_Blocks.front();
      memcpy(pcTarget + offset, p, m_BlockSize);
      bytesLeft -= m_BlockSize;
      offset += m_BlockSize;
      m_Blocks.pop_front();
      _releaseBlock(p);
    }

    //a_Now we have less than a block to go
    if (bytesLeft > 0)
    {
      if (m_Blocks.size() > 0)
      {
        //a_Use front block after poping it
        _popFrontBlock();
        memcpy(pcTarget + offset, mp_FrontBlock, bytesLeft);
        m_FrontBlockFree += bytesLeft;
      }
      else
      {
        //a_Use back block
        memcpy(pcTarget + offset, mp_BackBlock, bytesLeft);

        for (size_t j = bytesLeft; j + m_BackBlockFree < m_BlockSize; ++j)
          *(mp_BackBlock + j - bytesLeft) = *(mp_BackBlock + j);
        m_BackBlockFree += bytesLeft;
      }
    }

    return retBytes;
  }
}

size_t ARopeDeque::removeFront(size_t bytesToRemove)
{
  size_t bytesAvailable = getSize();
  if (bytesToRemove > bytesAvailable)
  {
    //a_Requested more data than we have
    clear();
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesToRemove;
    if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
    {
      size_t bytes = (bytesToRemove + m_FrontBlockFree > m_BlockSize ? m_BlockSize - m_FrontBlockFree : bytesToRemove);
      bytesToRemove -= bytes;
      m_FrontBlockFree += bytes;
    }

    //a_Loop until less than block left
    while (bytesToRemove > m_BlockSize)
    {
      AASSERT(this, m_Blocks.size() > 0);
      bytesToRemove -= m_BlockSize;
      char *p = m_Blocks.front();
      m_Blocks.pop_front();
      _releaseBlock(p);
    }

    //a_Now we have less than a block to go
    if (bytesToRemove > 0)
    {
      if (m_Blocks.size() > 0)
      {
        //a_Use front block after poping it
        _popFrontBlock();
        m_FrontBlockFree += bytesToRemove;
      }
      else
      {
        //a_Use back block
        for (size_t j = bytesToRemove; j < m_BlockSize; ++j)
          *(mp_BackBlock + j - bytesToRemove) = *(mp_BackBlock + j);
        m_BackBlockFree += bytesToRemove;
      }
    }

    return retBytes;
  }
}

void ARopeDeque::pushFront(const char *pccBuffer, size_t bytesLeft)
{
  while (bytesLeft > 0)
  {
    if (!m_FrontBlockFree)
      _newFrontBlock();

    if (bytesLeft > m_FrontBlockFree)
    {
      //a_More bytes that what we have in the block
      bytesLeft -= m_FrontBlockFree;
      memcpy(mp_FrontBlock, pccBuffer + bytesLeft, m_FrontBlockFree);
      m_FrontBlockFree = 0;
    }
    else
    {
      //a_Block is bigger than bytes left
      memcpy(mp_FrontBlock + m_FrontBlockFree - bytesLeft, pccBuffer, bytesLeft);
      m_FrontBlockFree -= bytesLeft;
      bytesLeft = 0;
    }
  }
}

void ARopeDeque::pushBack(const char *pccBuffer, size_t bytesLeft)
{
  size_t sourcePos = 0;
  while (bytesLeft > 0)
  {
    if (!m_BackBlockFree)
      _newBackBlock();  
    
    if (bytesLeft > m_BackBlockFree)
    {
      //a_Write whatever we can in the current buffer
      memcpy(mp_BackBlock + m_BlockSize - m_BackBlockFree, pccBuffer + sourcePos, m_BackBlockFree);
      sourcePos += m_BackBlockFree;
      bytesLeft -= m_BackBlockFree;
      m_BackBlockFree = 0;
    }
    else
    {
      //a_What is left can fit in the current buffer
      memcpy(mp_BackBlock + m_BlockSize - m_BackBlockFree, pccBuffer + sourcePos, bytesLeft);
      m_BackBlockFree -= bytesLeft;
      bytesLeft = 0;
    }
  }
}

size_t ARopeDeque::peekBack(char *pcTarget, size_t bytesLeft) const
{
  size_t bytesAvailable = getSize();
  if (bytesLeft > bytesAvailable)
  {
    //a_Requested more data than we have
    _peekAll(pcTarget);
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesLeft;
    size_t offset = bytesLeft;
    
    if (mp_BackBlock && m_BlockSize > m_BackBlockFree)
    {
      size_t bytes = (bytesLeft + m_BackBlockFree > m_BlockSize ? m_BlockSize - m_BackBlockFree : bytesLeft);
      offset -= bytes;
      memcpy(pcTarget + offset, mp_BackBlock + m_BlockSize - m_BackBlockFree - bytes, bytes);
      bytesLeft -= bytes;
    }

    //a_Loop through blocks
    BlockContainer::const_reverse_iterator cit = m_Blocks.rbegin();
    while (bytesLeft > m_BlockSize)
    {
      offset -= m_BlockSize;
      memcpy(pcTarget + offset, (*cit), m_BlockSize);
      bytesLeft -= m_BlockSize;
      ++cit;
    }

    //a_Now we have less than a block to go
    if (bytesLeft > 0)
    {
      offset -= bytesLeft;
      if (cit != m_Blocks.rend())
      {
        //a_We have more blocks for partial read
        memcpy(pcTarget + offset, (*cit), bytesLeft);
      }
      else
      {
        //a_Use front block
        memcpy(pcTarget + offset, mp_FrontBlock + m_BlockSize - bytesLeft, bytesLeft);
      }
    }
    return retBytes;
  }
}

size_t ARopeDeque::peekFront(char *pcTarget, size_t bytesLeft) const
{
  size_t bytesAvailable = getSize();
  size_t offset = 0;
  if (bytesLeft > bytesAvailable)
  {
    //a_Requested more data than we have
    _peekAll(pcTarget);
    return bytesAvailable;
  }
  else
  {
    //a_We have enough
    size_t retBytes = bytesLeft;
    if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
    {
      size_t bytes = (bytesLeft + m_FrontBlockFree > m_BlockSize ? m_BlockSize - m_FrontBlockFree : bytesLeft);
      memcpy(pcTarget + offset, mp_FrontBlock + m_FrontBlockFree, bytes);
      bytesLeft -= bytes;
      offset += bytes;
    }

    //a_Loop until less than block left
    BlockContainer::const_iterator cit = m_Blocks.begin();
    while (cit != m_Blocks.end() && bytesLeft > m_BlockSize)
    {
      memcpy(pcTarget + offset, (*cit), m_BlockSize);
      bytesLeft -= m_BlockSize;
      offset += m_BlockSize;
      ++cit;
    }

    //a_Now we have less than a block to go
    if (bytesLeft > 0)
    {
      if (cit != m_Blocks.end())
      {
        //a_We have more blocks for partial read
        memcpy(pcTarget + offset, (*cit), bytesLeft);
      }
      else
      {
        //a_Use back block
        memcpy(pcTarget + offset, mp_BackBlock, bytesLeft);
      }
    }

    return retBytes;
  }
}

void ARopeDeque::_newFrontBlock()
{
  AASSERT(this, m_FrontBlockFree == 0);
  AASSERT(this, getSize() < DEBUG_MAXSIZE_ARopeDeque);  //a_Debug only limit

  if (m_FrontBlockFree == 0)
  {
    if (mp_FrontBlock)
      m_Blocks.push_front(mp_FrontBlock);

    if (m_FreeStore.size() > 0)
    {
      mp_FrontBlock = m_FreeStore.back();
      m_FreeStore.pop_back();
    }
    else
      mp_FrontBlock = new char[m_BlockSize];
    
    m_FrontBlockFree = m_BlockSize;
  }
}

void ARopeDeque::_newBackBlock()
{
  AASSERT(this, m_BackBlockFree == 0);
  AASSERT(this, getSize() < DEBUG_MAXSIZE_ARopeDeque);  //a_Debug only limit

  if (m_BackBlockFree == 0)
  {
    if (mp_BackBlock)
      m_Blocks.push_back(mp_BackBlock);

    if (m_FreeStore.size() > 0)
    {
      mp_BackBlock = m_FreeStore.back();
      m_FreeStore.pop_back();
    }
    else
      mp_BackBlock = new char[m_BlockSize];
    
    m_BackBlockFree = m_BlockSize;
  }
}

void ARopeDeque::_popFrontBlock()
{
//  AASSERT(this, m_FrontBlockFree == m_BlockSize);
  if (m_Blocks.size() > 0)
  {
    _releaseBlock(mp_FrontBlock);

    mp_FrontBlock = m_Blocks.front();
    m_Blocks.pop_front();
  }
  else
  {
    _newFrontBlock();
  }
  m_FrontBlockFree = 0;
}

void ARopeDeque::_popBackBlock()
{
//  AASSERT(this, m_BackBlockFree == m_BlockSize);
  if (m_Blocks.size() > 0)
  {
    _releaseBlock(mp_BackBlock);

    mp_BackBlock = m_Blocks.back();
    m_Blocks.pop_back();
  }
  else
  {
    _newBackBlock();
  }
  m_BackBlockFree = 0;
}

void ARopeDeque::_releaseBlock(char *pBlock)
{
  if (pBlock)
    m_FreeStore.push_back(pBlock);
}

void ARopeDeque::_popAll(char *pcTarget)
{
  //a_Copy everything, request > than data available
  size_t offset = 0;
  if (mp_FrontBlock && m_BlockSize - m_FrontBlockFree > 0)
  {
    memcpy(pcTarget, mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
    offset += m_BlockSize - m_FrontBlockFree;
    m_FrontBlockFree = m_BlockSize;
  }

  BlockContainer::iterator it = m_Blocks.begin();
  while (it != m_Blocks.end())
  {
    memcpy(pcTarget + offset, (*it), m_BlockSize);
    offset += m_BlockSize;
    m_FreeStore.push_back(*it);
    ++it;
  }
  m_Blocks.clear();

  if (mp_BackBlock && m_BlockSize - m_BackBlockFree > 0)
  {
    memcpy(pcTarget + offset, mp_BackBlock, m_BlockSize - m_BackBlockFree);
    m_BackBlockFree = m_BlockSize;
  }
}

void ARopeDeque::_peekAll(char *pcTarget) const
{
  //a_Copy everything, request > than data available, do not remove any of the data
  size_t offset = 0;
  if (mp_FrontBlock && m_BlockSize - m_FrontBlockFree > 0)
  {
    memcpy(pcTarget, mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
    offset += m_BlockSize - m_FrontBlockFree;
  }

  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    memcpy(pcTarget + offset, (*cit), m_BlockSize);
    offset += m_BlockSize;
    ++cit;
  }

  if (mp_BackBlock && m_BlockSize - m_BackBlockFree > 0)
  {
    memcpy(pcTarget + offset, mp_BackBlock, m_BlockSize - m_BackBlockFree);
  }
}

size_t ARopeDeque::peekFrontUntilOneOf(AString& strTarget, const AString& strSet) const
{
  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(mp_FrontBlock[i]))
      {
        //a_Found in front block
        strTarget.append(mp_FrontBlock + m_FrontBlockFree, i - m_FrontBlockFree);
        return i - m_FrontBlockFree;
      }
    }
  }

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(*((*cit)+i)))
      {
        //a_Found in this block
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        { 
          strTarget.append(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
          retSize = m_BlockSize - m_FrontBlockFree;
        }

        BlockContainer::const_iterator cit2 = m_Blocks.begin();
        while (cit2 != cit)
        {
          strTarget.append((*cit2), m_BlockSize);
          retSize += m_BlockSize;
          ++cit2;
        }
        strTarget.append((*cit), i);

        return retSize + i;
      }
    }
    ++cit;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (
            AConstant::npos != strSet.find(mp_BackBlock[i]) ||
            m_BlockSize <= m_BackBlockFree + i
         )
      {
        //a_Copy everything up to this point
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        { 
          strTarget.append(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
          retSize = m_BlockSize - m_FrontBlockFree;
        }

        //a_Conatiner
        BlockContainer::const_iterator cit2 = m_Blocks.begin();
        while (cit2 != m_Blocks.end())
        {
          strTarget.append((*cit2), m_BlockSize);
          retSize += m_BlockSize;
          ++cit2;
        }

        //a_Back block
        strTarget.append(mp_BackBlock, i);


        return retSize + i;
      }
    }
  }
  return AConstant::npos;
}

size_t ARopeDeque::popFrontUntilOneOf(
  AString& strTarget,
  const AString& strSet,
  bool boolRemoveDelimiter /* = true */,
  bool boolDiscardDelimiter /* = false */
)
{
  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(mp_FrontBlock[i]))
      {
        //a_Found in front block
        if (i > m_FrontBlockFree)
          strTarget.append(mp_FrontBlock + m_FrontBlockFree, i - m_FrontBlockFree + (boolRemoveDelimiter && !boolDiscardDelimiter ? 1 : 0));

        size_t retSize = i - m_FrontBlockFree + (boolRemoveDelimiter && !boolDiscardDelimiter ? 1 : 0);
        m_FrontBlockFree = i + (boolRemoveDelimiter ? 1 : 0);

        return retSize;
      }
    }
  }

  //a_Check container blocks
  BlockContainer::iterator it = m_Blocks.begin();
  while (it != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(*((*it)+i)))
      {
        //a_Found in this block, copy front first
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        {
          strTarget.append(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
          retSize = m_BlockSize - m_FrontBlockFree;
          m_FrontBlockFree = m_BlockSize;
        }

        BlockContainer::iterator it2 = m_Blocks.begin();
        while (it2 != it)
        {
          strTarget.append((*it2), m_BlockSize);
          retSize += m_BlockSize;
          _releaseBlock(*it2);
          ++it2;
        }
        m_Blocks.erase(m_Blocks.begin(), it2);

        //a_Move top block to front block
        _popFrontBlock();
        strTarget.append(mp_FrontBlock, i + (boolRemoveDelimiter && !boolDiscardDelimiter ? 1 : 0));
        m_FrontBlockFree = i + (boolRemoveDelimiter ? 1 : 0);

        return retSize + i + (boolRemoveDelimiter && !boolDiscardDelimiter ? 1 : 0);
      }
    }
    ++it;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (
            AConstant::npos != strSet.find(mp_BackBlock[i]) ||
            m_BlockSize <= m_BackBlockFree + i
         )
      {
        //a_Copy everything up to this point
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        { 
          strTarget.append(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
          retSize = m_BlockSize - m_FrontBlockFree;
          m_FrontBlockFree = m_BlockSize;
        }

        //a_Conatiner
        BlockContainer::iterator it2 = m_Blocks.begin();
        while (it2 != m_Blocks.end())
        {
          strTarget.append((*it2), m_BlockSize);
          retSize += m_BlockSize;
          _releaseBlock(*it2);
          ++it2;
        }
        m_Blocks.clear();

        //a_Back block
        AASSERT(this, m_BlockSize > m_BackBlockFree);

        strTarget.append(mp_BackBlock, i + (boolRemoveDelimiter && !boolDiscardDelimiter ? 1 : 0));

        if (boolRemoveDelimiter)
          ++i;

        for (size_t j = i; j + m_BackBlockFree < m_BlockSize; ++j)
          *(mp_BackBlock + j - i) = *(mp_BackBlock + j);
        m_BackBlockFree += i;

        return retSize + i - (boolRemoveDelimiter && boolDiscardDelimiter ? 1 : 0);
      }
    }
  }

  return AConstant::npos;
}

size_t ARopeDeque::removeFrontUntilOneOf(
  const AString& strSet,
  bool boolRemoveDelimiter /* = true */
)
{
  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(mp_FrontBlock[i]))
      {
        //a_Found in front block
        size_t retSize = i - m_FrontBlockFree + (boolRemoveDelimiter ? 1 : 0);
        m_FrontBlockFree = i;
        if (boolRemoveDelimiter)
          removeFront(1);

        return retSize;
      }
    }
  }

  //a_Check container blocks
  BlockContainer::iterator it = m_Blocks.begin();
  while (it != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (AConstant::npos != strSet.find(*((*it)+i)))
      {
        //a_Found in this block, copy front first
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        {
          retSize = m_BlockSize - m_FrontBlockFree;
          m_FrontBlockFree = m_BlockSize;
        }

        BlockContainer::iterator it2 = m_Blocks.begin();
        while (it2 != it)
        {
          retSize += m_BlockSize;
          _releaseBlock(*it2);
          ++it2;
        }
        m_Blocks.erase(m_Blocks.begin(), it2);

        //a_Move top block to front block
        _popFrontBlock();
        m_FrontBlockFree = i;

        if (boolRemoveDelimiter)
          removeFront(1);

        return retSize + i + (boolRemoveDelimiter ? 1 : 0);
      }
    }
    ++it;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (
            AConstant::npos != strSet.find(mp_BackBlock[i]) ||
            m_BlockSize <= m_BackBlockFree + i
         )
      {
        //a_Copy everything up to this point
        size_t retSize = 0;
        if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
        { 
          retSize = m_BlockSize - m_FrontBlockFree;
          m_FrontBlockFree = m_BlockSize;
        }

        //a_Conatiner
        BlockContainer::iterator it2 = m_Blocks.begin();
        while (it2 != m_Blocks.end())
        {
          retSize += m_BlockSize;
          _releaseBlock(*it2);
          ++it2;
        }
        m_Blocks.clear();

        //a_Back block
        if (boolRemoveDelimiter)
          ++i;

        for (size_t j = i; j + m_BackBlockFree < m_BlockSize ; ++j)
          *(mp_BackBlock + j - i) = *(mp_BackBlock + j);
        m_BackBlockFree += i;

        return retSize + i;
      }
    }
  }
  return AConstant::npos;
}

size_t ARopeDeque::peekFrontUntil(AString& strTarget, const AString& strPattern) const
{
  size_t pos = 0;            //a_Offset into pattern if partial find

  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_FrontBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found it in the front block
          strTarget.assign(mp_FrontBlock+m_FrontBlockFree, i-pos);
          return strTarget.getSize();
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == *((*cit)+i))
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found among the middle blocks
          //a_Get front
          if (mp_FrontBlock)
            strTarget.assign(mp_FrontBlock+m_FrontBlockFree, m_BlockSize-m_FrontBlockFree);
          
          //a_Get middle with pattern
          BlockContainer::const_iterator cit2 = m_Blocks.begin();
          while (cit2 != cit)
          {
            strTarget.append(*cit2, m_BlockSize);
            ++cit2;
          }
          strTarget.append(*cit2, i+1);

          //a_Remove pattern
          strTarget.rremove(strPattern.getSize());

          return strTarget.getSize();
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }   
    ++cit;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_BackBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found in back block
          //a_Get front
          if (mp_FrontBlock)
            strTarget.assign(mp_FrontBlock+m_FrontBlockFree, m_BlockSize-m_FrontBlockFree);
          
          //a_Get all of middle
          BlockContainer::const_iterator cit2 = m_Blocks.begin();
          while (cit2 != m_Blocks.end())
          {
            strTarget.append(*cit2, m_BlockSize);
            ++cit2;
          }

          strTarget.append(mp_BackBlock, i+1);
          
          //a_Remove pattern
          strTarget.rremove(strPattern.getSize());

          return strTarget.getSize();
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }
  return AConstant::npos;
}

size_t ARopeDeque::find(const AString& strPattern) const
{
  size_t pos = 0;            //a_Offset into pattern if partial find

  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_FrontBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found it in the front block
          return i - m_FrontBlockFree - pos;
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }

  //a_Check container blocks
  size_t blockNum = 0;
  size_t posFindStart = 0, blockFindStart = -1;
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == *((*cit)+i))
      {
        if (0 == pos)
        {
          if (0 == i)
          {
            //a_Pattern started matching in last block, since this match means we are past 1 char
            blockFindStart = blockNum - 1;
            posFindStart = m_BlockSize - 1;
          }
          else
          {
            //a_Started matching before this char
            blockFindStart = blockNum;
            posFindStart = i - 1;
          }
        }
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found among the middle blocks
          size_t ret = blockFindStart * m_BlockSize + posFindStart;
          if (mp_FrontBlock)
            ret += m_BlockSize - m_FrontBlockFree;
          return ret;
        }
      }
      else
      {
        pos = 0;  //a_No match, reset search pattern
      }
    }   
    ++cit;
    ++blockNum;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_BackBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found in back block
          size_t ret = m_Blocks.size() * m_BlockSize + i - pos;
          if (mp_FrontBlock)
            ret += m_BlockSize - m_FrontBlockFree;
          return ret;
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }
  return AConstant::npos;
}

size_t ARopeDeque::popFrontUntil(
  AString& strTarget,
  const AString& strPattern,
  bool boolRemovePattern /* = true */,
  bool boolDiscardPattern /* = false */
)
{
  if (strPattern.isEmpty())
    ATHROW(this, AException::InvalidParameter);
  
  //a_Find start character
  size_t length = getSize();
  if (length >= strPattern.getSize())
  {
    size_t currentPos = 0;
    size_t startPos;
    while (AConstant::npos != (startPos = find(strPattern.at(0), currentPos)))
    {
      if (0 == compare(strPattern, startPos))
      {
        //a_Found it
        if (boolRemovePattern)
        {
          if (boolDiscardPattern)
          {
            size_t ret = popFront(strTarget, startPos);
            removeFront(strPattern.getSize());
            return ret;
          }
          else
          {
            return popFront(strTarget, startPos+strPattern.getSize());
          }
        }
        else 
        {
          return popFront(strTarget, startPos);
        }
      }
      currentPos = startPos + 1;
    }
  }

  return AConstant::npos;
}

size_t ARopeDeque::find(char c, size_t startPos /* = 0 */) const
{
  if (AConstant::npos == startPos)
    ATHROW(this, AException::InvalidParameter);

  size_t pos = 0;
  
  //a_Check front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    if (startPos + m_FrontBlockFree >= m_BlockSize)
    {
      //a_Skip over the front block
      pos += m_BlockSize - m_FrontBlockFree;
      startPos -= m_BlockSize - m_FrontBlockFree;
    }
    else
    {
      pos += startPos;
      for(size_t i=m_FrontBlockFree+startPos; i<m_BlockSize; ++i, ++pos)
      {
        if (c == mp_FrontBlock[i])
          return pos;
      }
      startPos = 0;
    }
  }
  
  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    if (startPos > m_BlockSize)
    {
      pos += m_BlockSize;
      startPos -= m_BlockSize;
    }
    else
    {
      pos += startPos;
      for (size_t i=startPos; i<m_BlockSize; ++i, ++pos)
      {
        if (c == *((*cit)+i))
        {
          return pos;
        }
      }
      startPos = 0;
    }
    ++cit;
  }
  
  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    AASSERT(this, startPos + m_BackBlockFree <= m_BlockSize);
    pos += startPos;
    for(size_t i = startPos; i + m_BackBlockFree < m_BlockSize; ++i, ++pos)
    {
      if (c == mp_BackBlock[i])
      {
        return pos;
      }
    }
  }
  
  return AConstant::npos;
}

size_t ARopeDeque::compare(const AString& strPattern, size_t startPos /* = 0 */) const
{
  if (AConstant::npos == startPos)
    ATHROW(this, AException::InvalidParameter);

  size_t pos = 0;
  
  //a_Check front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    if (startPos + m_FrontBlockFree >= m_BlockSize)
    {
      //a_Skip over the front block
      startPos -= m_BlockSize - m_FrontBlockFree;
    }
    else
    {
      for(size_t i=m_FrontBlockFree+startPos; i<m_BlockSize && pos<strPattern.getSize(); ++i)
      {
        if (strPattern.at(pos++) != mp_FrontBlock[i])
          return startPos + pos;
      }
      startPos = 0;
    }
  }
  
  if (pos == strPattern.getSize())
    return 0;

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end() && pos < strPattern.getSize())
  {
    if (startPos > m_BlockSize)
    {
      startPos -= m_BlockSize;
    }
    else
    {
      for (size_t i=startPos; i<m_BlockSize && pos<strPattern.getSize(); ++i)
      {
        if (strPattern.at(pos++) != *((*cit)+i))
          return startPos + pos;
      }
      startPos = 0;
    }
    
    if (pos == strPattern.getSize())
      return 0;

    ++cit;
  }
  
  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize && pos < strPattern.getSize())
  {
    AASSERT(this, startPos + m_BackBlockFree < m_BlockSize);
    for(size_t i = startPos; i + m_BackBlockFree < m_BlockSize && pos<strPattern.getSize(); ++i)
    {
      if (strPattern.at(pos++) != mp_BackBlock[i])
      {
        return startPos + pos;
      }
    }
  }
  
  if (pos == strPattern.getSize())
    return 0;              //a_Found
  else
    return AConstant::npos;  //a_Incomplete match
}

size_t ARopeDeque::removeFrontUntil(const AString& strPattern, bool boolRemovePattern /* = true */)
{
  size_t pos = 0;            //a_Offset into pattern if partial find

  //a_Start searching in the front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for (size_t i = m_FrontBlockFree; i < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_FrontBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found it in the front block
          size_t retSize = (i + 1 - m_FrontBlockFree) + (boolRemovePattern ? 0 : pos);

          m_FrontBlockFree = i + 1 - (boolRemovePattern ? 0 : pos);

          return retSize;
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }

  //a_Check container blocks
  BlockContainer::iterator it = m_Blocks.begin();
  while (it != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == *((*it)+i))
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found among the middle blocks
          size_t retSize = 0;

          //a_Remove front
          if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
          {
            retSize = m_BlockSize - m_FrontBlockFree;
            m_FrontBlockFree = m_BlockSize;
          }
          
          //a_Remove middle with pattern
          BlockContainer::iterator it2 = m_Blocks.begin();
          while (it2 != it)
          {
            retSize += m_BlockSize;
            _releaseBlock(*it2);
            ++it2;
          }
          m_Blocks.erase(m_Blocks.begin(), it2);

          //a_Move top block to front block
          _popFrontBlock();
          m_FrontBlockFree = i + 1;

          if (!boolRemovePattern)
            pushFront(strPattern);

          return retSize + i + 1 - (boolRemovePattern ? 0 : pos);
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }   
    ++it;
  }

  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i = 0; i + m_BackBlockFree < m_BlockSize; ++i)
    {
      if (strPattern.at(pos) == mp_BackBlock[i])
      {
        ++pos;
        if (pos == strPattern.getSize())
        {
          //a_Found in back block
          size_t retSize = 0;

          //a_Remove front
          if (mp_FrontBlock && m_BlockSize > m_FrontBlockFree)
          {
            retSize = m_BlockSize - m_FrontBlockFree;
            m_FrontBlockFree = m_BlockSize;
          }
          
          //a_Remove middle
          BlockContainer::iterator it2 = m_Blocks.begin();
          while (it2 != m_Blocks.end())
          {
            retSize += m_BlockSize;
            _releaseBlock(*it2);
            ++it2;
          }
          m_Blocks.clear();

          ++i;
          for (size_t j = i; j + m_BackBlockFree < m_BlockSize; ++j)
            *(mp_BackBlock + j - i) = *(mp_BackBlock + j);
          m_BackBlockFree += i;

          if (!boolRemovePattern)
            pushFront(strPattern);

          return retSize + i - (boolRemovePattern ? 0 : pos);
        }
      }
      else
        pos = 0;  //a_No match, reset search pattern
    }
  }
  return AConstant::npos;
}

size_t ARopeDeque::_append(const char *pcc, size_t len)
{
  if (AConstant::npos == len)
    len = strlen(pcc);

  pushBack(pcc, len);
  return len;
}

size_t ARopeDeque::peekFrontUntil(
  AString& target, 
  char delim, 
  size_t maxBytes              // = AConstant::npos
) const
{
  size_t pos = 0;
  
  //a_Check front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for(size_t i=m_FrontBlockFree; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == mp_FrontBlock[i])
      {
        peekFront(target, pos);
        return pos;
      }
    }
    if (pos == maxBytes)
      return maxBytes;
  }

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == *((*cit)+i))
      {
        peekFront(target, pos);
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;

    ++cit;
  }
  
  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i=0; ((i+m_BackBlockFree)<m_BlockSize) && (pos<maxBytes); ++i, ++pos)
    {
      if (delim == mp_BackBlock[i])
      {
        peekFront(target, pos);
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;
  }

  return AConstant::npos;
}

size_t ARopeDeque::popFrontUntil(
  AString& target, 
  char delim, 
  bool boolRemovePattern,  // = true 
  bool discardPattern,     // = false 
  size_t maxBytes              // = AConstant::npos
)
{
  size_t pos = 0;
  
  //a_Check front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for(size_t i=m_FrontBlockFree; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == mp_FrontBlock[i])
      {
        popFront(target, pos);
        if (boolRemovePattern)
        {
          removeFront(1);
          if (!discardPattern)
          {
            target.append(delim);
            ++pos;
          }
        }
        return pos;
      }
    }
    if (pos == maxBytes)
      return maxBytes;
  }

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == *((*cit)+i))
      {
        popFront(target, pos);
        if (boolRemovePattern)
        {
          removeFront(1);
          if (!discardPattern)
          {
            target.append(delim);
            ++pos;
          }
        }
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;

    ++cit;
  }
  
  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i=0; ((i+m_BackBlockFree)<m_BlockSize) && (pos<maxBytes); ++i, ++pos)
    {
      if (delim == mp_BackBlock[i])
      {
        popFront(target, pos);
        if (boolRemovePattern)
        {
          removeFront(1);
          if (!discardPattern)
          {
            target.append(delim);
            ++pos;
          }
        }
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;
  }

  return AConstant::npos;
}

size_t ARopeDeque::removeFrontUntil(
  char delim, 
  bool boolRemovePattern, // = true 
  size_t maxBytes             // = AConstant::npos
)
{
  size_t pos = 0;
  
  //a_Check front block
  if (mp_FrontBlock && m_FrontBlockFree < m_BlockSize)
  {
    for(size_t i=m_FrontBlockFree; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == mp_FrontBlock[i])
      { 
        pos += (boolRemovePattern ? 1 : 0);
        removeFront(pos);
        return pos;
      }
    }
    if (pos == maxBytes)
      return maxBytes;
  }

  //a_Check container blocks
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    for (size_t i=0; i<m_BlockSize && pos<maxBytes; ++i, ++pos)
    {
      if (delim == *((*cit)+i))
      {
        pos += (boolRemovePattern ? 1 : 0);
        removeFront(pos);
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;

    ++cit;
  }
  
  //a_Check back block
  if (mp_BackBlock && m_BackBlockFree < m_BlockSize)
  {
    for(size_t i=0; ((i+m_BackBlockFree)<m_BlockSize) && (pos<maxBytes); ++i, ++pos)
    {
      if (delim == mp_BackBlock[i])
      {
        pos += (boolRemovePattern ? 1 : 0);
        removeFront(pos);
        return pos;
      }
    }

    if (pos == maxBytes)
      return maxBytes;
  }

  return AConstant::npos;
}

size_t ARopeDeque::flush(AFile& file)
{
  size_t totalBytesWritten = 0;
  if (mp_FrontBlock)
  {
    size_t ret = file.write(mp_FrontBlock + m_FrontBlockFree, m_BlockSize - m_FrontBlockFree);
    if (AConstant::npos == ret || AConstant::unavail == ret)
      return ret;

    totalBytesWritten += ret;
    m_FreeStore.push_back(mp_FrontBlock);
    mp_FrontBlock = NULL;
    m_FrontBlockFree = 0;
  }

  while (m_Blocks.size() > 0)
  {
    char *pBlock = m_Blocks.front();
    size_t ret = file.write(pBlock, m_BlockSize);
    if (AConstant::npos == ret || AConstant::unavail == ret)
      return (totalBytesWritten > 0 ? totalBytesWritten : ret);

    totalBytesWritten += ret;
    m_Blocks.pop_front();
    m_FreeStore.push_back(pBlock);
  }

  if (mp_BackBlock)
  {
    size_t ret = file.write(mp_BackBlock, m_BlockSize - m_BackBlockFree);
    if (AConstant::npos == ret || AConstant::unavail == ret)
      return (totalBytesWritten > 0 ? totalBytesWritten : ret);

    totalBytesWritten += ret;
    m_FreeStore.push_back(mp_BackBlock);
    mp_BackBlock = NULL;
    m_BackBlockFree = 0;
  }

  file.flush();
  return totalBytesWritten;
}

