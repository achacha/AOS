
#include "pchABase.hpp"

#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ASystemException.hpp"
#include "AFilename.hpp"
#include <io.h>
#include <limits.h>

const AString AFile_Physical::sm_DefaultOpenFlags("rb");          //a_Read binary file by default
const int AFile_Physical::sm_DefaultShareFlags = _SH_DENYNO;

void AFile_Physical::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "m_filename=" << m_filename << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_openflags=" << m_openflags << "  m_shareflags=" << m_shareflags << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_fid=" << m_fid << "  mp_file=" << AString::fromPointer(mp_file) << std::endl;

  AFile::debugDump(os, indent+1);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFile_Physical::AFile_Physical() :
  mp_file(NULL)
{
}

AFile_Physical::AFile_Physical(
  const AString& filename,
  const AString& openflags,  // = sm_DefaultOpenFlags
  int shareflags             // = sm_DefaultShareFlags
) :
  mp_file(NULL),
  m_fid(-1),
  m_filename(filename, false),
  m_openflags(openflags),
  m_shareflags(shareflags)
{
}

AFile_Physical::AFile_Physical(
  const AFilename& filename,
  const AString& openflags,  // = sm_DefaultOpenFlags
  int shareflags             // = sm_DefaultShareFlags
) :
  mp_file(NULL),
  m_fid(-1),
  m_filename(filename),
  m_openflags(openflags),
  m_shareflags(shareflags)
{
}

AFile_Physical::~AFile_Physical()
{
  close();
}

void AFile_Physical::open()
{
  if (mp_file)
    close();

  const AString& filename = m_filename.toAString();
  const char *pccOpenFlags = m_openflags.c_str();
  mp_file = ::_fsopen(filename.c_str(), pccOpenFlags, m_shareflags);
  if (!mp_file)
  {  
    ATHROW_ERRNO_EX(this, AException::UnableToOpen, m_filename, errno);
  }

  m_fid = _fileno(mp_file);
}

void AFile_Physical::close()
{
  if (mp_file)
  {
    ::fclose(mp_file);
    mp_file = NULL;
    m_fid = -1;
  }
}

size_t AFile_Physical::_write(const void *buf, size_t size)
{
  AASSERT_EX(this, m_fid >= 0, ASWNL("Probably forgot to call open() or invalid file handle after call to open"));   // m_fid == -1? forgot to call open()?
  if (mp_file)
  {
    size_t written = ::_write(m_fid, buf, size);
    if (AConstant::npos == written || written < size)
      ATHROW_ERRNO(this, AException::UnableToWrite, errno);
    
    return written;
  }
  else
    ATHROW(this, AException::NotOpen);
}

size_t AFile_Physical::_read(void *buf, size_t size)
{
  AASSERT_EX(this, m_fid >= 0, ASWNL("Probably forgot to call open() or invalid file handle after call to open"));   // m_fid == -1? forgot to call open()?
  if (mp_file)
  {
    size_t bytesread = ::_read(m_fid, buf, size);
    if (AConstant::npos == bytesread)
      ATHROW_ERRNO(this, AException::UnableToRead, errno);
    
    return bytesread;
  }
  else
    ATHROW(this, AException::NotOpen);
}

bool AFile_Physical::isOpen()
{
  return (NULL != mp_file && m_fid >= 0);
}

bool AFile_Physical::_isNotEof()
{
  if (mp_file)
    return (!_eof(m_fid));
  else
    ATHROW(this, AException::NotOpen);
}

void AFile_Physical::flush()
{
}

AFilename& AFile_Physical::useFilename()
{
  return m_filename;
}

AString& AFile_Physical::useOpenFlags()
{
  return m_openflags;
}

int& AFile_Physical::useShareFlags()
{
  return m_shareflags;
}

bool AFile_Physical::seek(u8 offset, int origin)
{
#ifdef _fseeki64
  return (!_fseeki64(mp_file, offset, origin));
#else
  return (!fseek(mp_file, (long)offset, origin));
#endif
}

u8 AFile_Physical::tell()
{
#ifdef _ftelli64
  return _ftelli64(mp_file);
#else
  return (u8)ftell(mp_file);
#endif
}

size_t AFile_Physical::getSize() const
{
  s8 size = AFileSystem::length(m_filename);
  AASSERT(this, size <= SIZE_MAX);             //a_If this ever happens getSize should be upgraded to s8
  return (size_t)size;
}
