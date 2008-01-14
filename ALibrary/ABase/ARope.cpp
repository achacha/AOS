#include "pchABase.hpp"
#include "ARope.hpp"
#include "AFile.hpp"

void ARope::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ARope @ " << std::hex << this << std::dec << ")" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "getSize()=" << getSize()
    << "  m_BlockSize=" << m_BlockSize
    << "  m_LastBlockFree=" << m_LastBlockFree
    << "  m_FreeStore.size()=" << m_FreeStore.size() << std::endl;
 
  if (m_Blocks.size() * m_BlockSize < 10240)
  {
    //a_Display if not too much
    ADebugDumpable::indent(os, indent+1) << "m_Blocks={" << std::endl;
    BlockContainer::const_iterator cit = m_Blocks.begin();
    while (cit != m_Blocks.end())
    {
      ADebugDumpable::dumpMemory_HexAscii(os, (*cit), m_BlockSize, indent+2);
      os << std::endl;
      ++cit;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
  {
    ADebugDumpable::indent(os, indent+1) << "Supressing content due to block count/size" << std::endl;
  }

  if (mp_LastBlock)
  {
    ADebugDumpable::indent(os, indent+1) << "mp_LastBlock=" << (void*)mp_LastBlock << " {" << std::endl;
    ADebugDumpable::dumpMemory_HexAscii(os, mp_LastBlock, m_BlockSize - m_LastBlockFree, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_LastBlock=NULL" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ARope::ARope(size_t blockSize /* = DEFAULT_BLOCK_SIZE */) :
  m_BlockSize(blockSize),
  m_LastBlockFree(0),
  mp_LastBlock(NULL)
{
}

ARope::ARope(const AString& str, size_t blockSize /* = DEFAULT_BLOCK_SIZE */) :
  m_BlockSize(blockSize),
  m_LastBlockFree(0),
  mp_LastBlock(NULL)
{
  append(str.c_str(), str.getSize());
}

ARope::ARope(const ARope& that) :
  m_BlockSize(that.m_BlockSize),
  m_LastBlockFree(that.m_LastBlockFree),
  mp_LastBlock(NULL)
{
  BlockContainer::const_iterator cit = that.m_Blocks.begin();
  while (cit != that.m_Blocks.end())
  {
    char *p = new char[m_BlockSize];
    memcpy(p, (*cit), m_BlockSize);
    m_Blocks.push_back(p);
    ++cit;
  }

  if (that.mp_LastBlock)
  {
    mp_LastBlock = new char[m_BlockSize];
    memcpy(mp_LastBlock, that.mp_LastBlock, m_BlockSize - m_LastBlockFree);
  }
}

ARope::~ARope()
{
  try
  {
    clear(true);
  }
  catch(...) {}  //a_Prevent exception propogation in dtor
}

bool ARope::isEmpty() const
{
  if (
    0 == m_Blocks.size()
    && (!mp_LastBlock || mp_LastBlock && m_LastBlockFree == m_BlockSize)
  )
    return true;
  else
    return false;
}

size_t ARope::getSize() const
{
  size_t ret = m_Blocks.size() * m_BlockSize;
  if (mp_LastBlock)
    ret += (m_BlockSize - m_LastBlockFree);

  return ret;
}

void ARope::clear(bool bReleaseMemory /* = false */)
{
  if (bReleaseMemory)
  {
    BlockContainer::iterator it = m_Blocks.begin();
    while (it != m_Blocks.end())
    {
      delete (*it);
      ++it;
    }
    m_Blocks.clear();

    it = m_FreeStore.begin();
    while(it != m_FreeStore.end())
    {
      delete (*it);
      ++it;
    }
    m_FreeStore.clear();

    pDelete(mp_LastBlock);
  }
  else
  {
    //a_Put blocks into storage
    BlockContainer::iterator it = m_Blocks.begin();
    while (it != m_Blocks.end())
    {
      m_FreeStore.push_back(*it);
      ++it;
    }
    m_Blocks.clear();

    if (mp_LastBlock)
    {
      m_FreeStore.push_back(mp_LastBlock);
      mp_LastBlock = NULL;
    }
  }
  m_LastBlockFree = 0;
}

void ARope::_append(const char *pSource, size_t bytesLeft)
{
  size_t sourcePos = 0;
  if (AConstant::npos == bytesLeft)
    bytesLeft = strlen(pSource);

  while (bytesLeft > 0)
  {
    if (!m_LastBlockFree)
      __newBlock();  
    
    if (bytesLeft > m_LastBlockFree)
    {
      //a_Write whatever we can in the current buffer
      memcpy(mp_LastBlock + m_BlockSize - m_LastBlockFree, pSource + sourcePos, m_LastBlockFree);
      sourcePos += m_LastBlockFree;
      bytesLeft -= m_LastBlockFree;
      m_LastBlockFree = 0;
    }
    else
    {
      //a_What is left can fit in the current buffer
      memcpy(mp_LastBlock + m_BlockSize - m_LastBlockFree, pSource + sourcePos, bytesLeft);
      m_LastBlockFree -= bytesLeft;
      bytesLeft = 0;
    }
  }
}

void ARope::__newBlock()
{
  AASSERT(this, m_LastBlockFree == 0);
  AASSERT(this, getSize() < DEBUG_MAXSIZE_ARope);  //a_Debug only limit

  if (mp_LastBlock)
    m_Blocks.push_back(mp_LastBlock);
  if (m_FreeStore.size() > 0)
  {
    //a_Reuse existing block
    mp_LastBlock = m_FreeStore.back();
    m_FreeStore.pop_back();
  }
  else
  {
    //a_Allocate new block
    mp_LastBlock = new char[m_BlockSize];
  }
  m_LastBlockFree = m_BlockSize;
}

void ARope::emit(AOutputBuffer& target) const
{
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    target.append((*cit), m_BlockSize);
    ++cit;
  }

  if (mp_LastBlock)
    target.append(mp_LastBlock, m_BlockSize - m_LastBlockFree);
}

AString ARope::toAString() const
{
  AString strRet;
  emit(strRet);
  return strRet;
}

size_t ARope::fromAFile(AFile& file, size_t length /* = AConstant::npos */)
{
  size_t bytesRead = 0;

  //a_Bytes to read is <= bytes free in last block
  if (length <= m_LastBlockFree)
  {
    bytesRead = file.read(mp_LastBlock + m_BlockSize - m_LastBlockFree, length);
    if (bytesRead == AConstant::npos)
      return 0;  //a_EOF, nothing read

    m_LastBlockFree -= bytesRead;
    return bytesRead;
  }
  
  size_t totalBytesRead = 0;
  while (length > 0)
  {
    bytesRead = file.read(mp_LastBlock + m_BlockSize - m_LastBlockFree, m_LastBlockFree);
    if (bytesRead == AConstant::npos)
      break;  //a_EOF
    
    m_LastBlockFree -= bytesRead;
    length -= bytesRead;
    totalBytesRead += bytesRead;

    if (!m_LastBlockFree)
      __newBlock();  
  }

  return totalBytesRead;
}

size_t ARope::toAFile(AFile& file) const
{
  size_t bytesWritten = 0;
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    bytesWritten += file.write((*cit), m_BlockSize);
    ++cit;
  }

  if (mp_LastBlock)
    bytesWritten += file.write(mp_LastBlock, m_BlockSize - m_LastBlockFree);

  return bytesWritten;
}

ARope& ARope::operator +(const AEmittable& source)
{
  source.emit(*this);
  return *this;
}

ARope& ARope::operator +(const char *source)
{
  append(source);
  return *this;
}

ARope& ARope::operator +(char data)
{
  append(data);
  return *this;
}

ARope& ARope::operator +=(const AEmittable& source)
{
  source.emit(*this);
  return *this;
}

ARope& ARope::operator +=(const char *source)
{
  append(source);
  return *this;
}

ARope& ARope::operator +=(char data)
{
  append(data);
  return *this;
}
