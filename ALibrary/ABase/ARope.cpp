#include "pchABase.hpp"
#include "ARope.hpp"
#include "AFile.hpp"

void ARope::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "getSize()=" << getSize()
    << "  m_BlockSize=" << m_BlockSize
    << "  m_LastBlockFree=" << m_LastBlockFree
    << std::endl;
 
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
    clear();
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

void ARope::clear()
{
  BlockContainer::iterator it = m_Blocks.begin();
  while (it != m_Blocks.end())
  {
    delete (*it);
    ++it;
  }
  m_Blocks.clear();

  pDelete(mp_LastBlock);

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

  //a_Allocate new block
  mp_LastBlock = new char[m_BlockSize];

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

size_t ARope::read(AFile& file, size_t length /* = AConstant::npos */)
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

size_t ARope::write(AFile& file) const
{
  size_t bytesTotalWritten = 0;
  BlockContainer::const_iterator cit = m_Blocks.begin();
  while (cit != m_Blocks.end())
  {
    size_t bytesToWrite = m_BlockSize;
    while (bytesToWrite)
    {
      size_t ret = file.write((*cit) + (m_BlockSize - bytesToWrite), bytesToWrite);
      
      bytesTotalWritten += ret;
      bytesToWrite -= ret;
    }
    ++cit;
  }

  if (mp_LastBlock && m_LastBlockFree < m_BlockSize)
  {
    size_t bytesToWrite = m_BlockSize - m_LastBlockFree;
    while (bytesToWrite)
    {
      size_t ret = file.write(mp_LastBlock + (m_BlockSize - bytesToWrite), bytesToWrite);
      bytesTotalWritten += ret;
      bytesToWrite -= ret;
    }
  }

  return bytesTotalWritten;
}

size_t ARope::flush(AFile& file)
{
  size_t bytesTotalWritten = 0;
  while (m_Blocks.size() > 0)
  {
    char *pBlock = m_Blocks.front();

    size_t bytesWritten = file.write(pBlock, m_BlockSize);
    if (AConstant::npos == bytesWritten || AConstant::unavail == bytesWritten)
    {
      if (bytesTotalWritten > 0)
        return bytesTotalWritten;   //a_Return partial written size
      else
        return bytesWritten;
    }
    AASSERT(this, bytesWritten);                 //a_Zero bytes written?
    AASSERT(this, bytesWritten == m_BlockSize);  //a_Unable to write the whole buffer?
    bytesTotalWritten += bytesWritten;

    //a_Release head block
    m_Blocks.pop_front();
    delete pBlock;
  }

  if (mp_LastBlock && m_LastBlockFree < m_BlockSize)
  {
    size_t bytesWritten = file.write(mp_LastBlock, m_BlockSize - m_LastBlockFree);
    if (AConstant::npos == bytesWritten || AConstant::unavail == bytesWritten)
    {
      if (bytesTotalWritten > 0)
        return bytesTotalWritten;   //a_Return partial written size
      else
        return bytesWritten;
    }
    AASSERT(this, bytesWritten);                 //a_Zero bytes written?
    AASSERT(this, bytesWritten == m_BlockSize - m_LastBlockFree);  //a_Unable to write the whole buffer?
    bytesTotalWritten += bytesWritten;

    //a_Release last block
    pDelete(mp_LastBlock);
  }

  if (!bytesTotalWritten)
    AASSERT(this, false);

  return bytesTotalWritten;
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
