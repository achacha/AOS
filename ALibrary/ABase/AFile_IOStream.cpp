#include "pchABase.hpp"
#include "AFile_IOStream.hpp"
#include "AException.hpp"

void AFile_IOStream::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFile_IOStream @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_IStream=" << std::hex << (void *)mp_IStream << std::dec << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_OStream=" << std::hex << (void *)mp_OStream << std::dec << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFile_IOStream& AFile_IOStream::operator>>(std::istream& (__CDECL * _f)(std::istream&)) { (*mp_IStream) >> _f; return *this; }
AFile_IOStream& AFile_IOStream::operator>>(std::ios& (__CDECL * _f)(std::ios&))         { (*mp_IStream) >> _f; return *this; }
AFile_IOStream& AFile_IOStream::operator<<(std::ostream& (__CDECL * _f)(std::ostream&)) { (*mp_OStream) << _f; return *this; }
AFile_IOStream& AFile_IOStream::operator<<(std::ios& (__CDECL * _f)(std::ios&))         { (*mp_OStream) << _f; return *this; }

AFile_IOStream::AFile_IOStream(std::ostream *pOstream, std::istream *pIStream) :
  mp_OStream(pOstream),
  mp_IStream(pIStream)
{
}

AFile_IOStream::AFile_IOStream(std::ostream& Ostream, std::istream& IStream) :
  mp_OStream(&Ostream),
  mp_IStream(&IStream)
{
}

AFile_IOStream::AFile_IOStream(const AFile_IOStream& aSource) :
  mp_OStream(aSource.mp_OStream),
  mp_IStream(aSource.mp_IStream)
{
}

size_t AFile_IOStream::_write(const void *pcvBuffer, size_t length)
{
  if (!mp_OStream)
    ATHROW(this, AException::OutputSourceInvalid);

  if (!pcvBuffer)
    ATHROW(this, AException::OutputBufferIsNull);

  if (mp_OStream->bad())
		return 0;

  mp_OStream->write((const char *)pcvBuffer, length);
  return length;
}

size_t AFile_IOStream::_read(void *pvBuffer, size_t length)
{
  if (!mp_IStream)
    ATHROW(this, AException::InputSourceInvalid);

  if (!pvBuffer)
    ATHROW(this, AException::InputBufferIsNull);

  if (mp_IStream->bad())
		return 0;

  mp_IStream->read((char *)pvBuffer, length);
	return mp_IStream->gcount();
}


void AFile_IOStream::flush()
{
  mp_OStream->flush();       //a_Flush only makes sense for the output
}

void AFile_IOStream::close()
{
  //a_Close does not apply to these classes
}

size_t AFile_IOStream::readBlockIntoLookahead()
{
  if (mp_IStream->rdstate() & std::ios::eofbit)
    return 0;
  
  size_t totalBytesRead = 0;
  size_t curpos = mp_IStream->tellg();
  if (curpos == -1)
  {
    if (!mp_IStream->rdbuf()->in_avail())
      mp_IStream->peek();

    while (mp_IStream->rdbuf()->in_avail())
    {
      char c = (char)mp_IStream->get();
      m_LookaheadBuffer.pushBack(&c, 1);
      ++totalBytesRead;
    }
    return totalBytesRead;
  }

  //a_Have a position in a file
  curpos = mp_IStream->tellg();
  mp_IStream->seekg(-1, std::ios_base::end);
  size_t endpos = mp_IStream->tellg();
  mp_IStream->seekg(curpos);
  
  AString str;
  size_t bytesToRead = endpos - curpos + 1;
  while (bytesToRead >= m_ReadBlock)
  {
    //a_Loop reading blocks until nothing left
    char *p = str.startUsingCharPtr(m_ReadBlock);
    size_t bytesRead = _read(p, m_ReadBlock);
    if (bytesRead > 0)
      m_LookaheadBuffer.pushBack(p, bytesToRead);

    totalBytesRead += bytesRead;
  }

  if (bytesToRead < m_ReadBlock)
  {
    //a_Less data than read block
    char *p = str.startUsingCharPtr(bytesToRead);
    size_t bytesRead = _read(p, bytesToRead);
    if (bytesRead > 0)
      m_LookaheadBuffer.pushBack(p, bytesRead);
    
    return totalBytesRead += bytesRead;

  }

  return totalBytesRead;
}

bool AFile_IOStream::_isNotEof()
{
  return !(mp_IStream->rdstate() & std::ios::eofbit);
}

bool AFile_IOStream::isOpen()
{
  return (mp_OStream->rdstate() == std::ios::goodbit);
}

AFile_IOStream::operator std::istream*()
{ 
  return mp_IStream;
}

AFile_IOStream::operator std::ostream*() 
{ 
  return mp_OStream; 
}

std::istream& AFile_IOStream::getIStream() { 
  if (mp_IStream)
    return *mp_IStream;
  else
    ATHROW_EX(this, AException::InvalidObject, ASWNL("std::istream is NULL"));
}
std::ostream& AFile_IOStream::getOStream()
{ 
  if(mp_OStream)
    return *mp_OStream;
  else
    ATHROW_EX(this, AException::InvalidObject, ASWNL("std::ostream is NULL"));
}
