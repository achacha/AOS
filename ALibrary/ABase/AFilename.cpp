/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AFilename.hpp"
#include "AXmlElement.hpp"

const AString AFilename::RESERVED("|\\?*<\":>/");
const AString AFilename::FILE_PREFIX("file:///");

void AFilename::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Type=" << m_Type;
  ADebugDumpable::indent(os, indent+1) << "  m_RelativePath=" << m_RelativePath << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Drive=" << m_Drive << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_PathNames={" << std::endl;
  LIST_AString::const_iterator cit = m_PathNames.begin();
  while (cit != m_PathNames.end())
  {
    ADebugDumpable::indent(os, indent+2) << *cit << std::endl;
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Filename=" << m_Filename << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFilename::AFilename() :
  m_Type(FTYPE_DEFAULT),
  m_Drive('\x0'),
  m_RelativePath(false)
{
}

AFilename::AFilename(const AFilename& filepath) :
  m_Type(filepath.m_Type),
  m_Drive(filepath.m_Drive),
  m_RelativePath(filepath.m_RelativePath),
  m_Filename(filepath.m_Filename)
{
  for (LIST_AString::const_iterator cit = filepath.m_PathNames.begin(); cit != filepath.m_PathNames.end(); ++cit)
  {
    m_PathNames.push_back(*cit);
  }
}

AFilename::AFilename(
  const AEmittable& filepath, 
  bool forceDirectory // = false
) :
  m_Type(FTYPE_DEFAULT),
  m_Drive('\x0'),
  m_RelativePath(false)
{
  set(filepath, forceDirectory);
}

AFilename::AFilename(
  const AEmittable& filepath0, 
  const AEmittable& filepath1, 
  bool forceDirectory
)
{
  set(filepath0, true);
  join(filepath1, forceDirectory);
}

AFilename::AFilename(
  const AEmittable& filepath0,
  const AEmittable& filepath1,
  const AEmittable& filepath2,
  bool forceDirectory
)
{
  set(filepath0, true);
  join(filepath1, forceDirectory);
  join(filepath2, forceDirectory);
}

AFilename::AFilename(
  const AFilename& filepath0, 
  const AFilename& filepath1 
)
{
  set(filepath0);
  join(filepath1);
}

AFilename::AFilename(
  const AFilename& filepath0,
  const AFilename& filepath1,
  const AFilename& filepath2
)
{
  set(filepath0);
  join(filepath1);
  join(filepath2);
}

void AFilename::set(const AFilename& filepath)
{
  m_Drive = filepath.m_Drive;
  m_Type = filepath.m_Type;
  m_RelativePath = filepath.m_RelativePath;
  m_Filename = filepath.m_Filename;
  m_PathNames.clear();
  for (LIST_AString::const_iterator cit = filepath.m_PathNames.begin(); cit != filepath.m_PathNames.end(); ++cit)
    m_PathNames.push_back(*cit);
}

void AFilename::set(
  const AEmittable& filepath,
  bool forceDirectory
)
{
  m_PathNames.clear();
  m_Filename.clear();

  AString temp(filepath);
  temp.replace('\\', '/');
  temp.split(m_PathNames, '/', AConstant::ASTRING_EMPTY, true);

  if (m_PathNames.empty())
  {
    //a_No path
    return;
  }

  //a_Check if drive letter is included
  AString& first = m_PathNames.front();
  if (first.getSize() > 1 && ':' == first.at(1))
  {
    m_Drive = (char)tolower(m_PathNames.front().at(0));
    first.remove(2);
    if (first.isEmpty())
      m_PathNames.pop_front();
  }
  else
    m_Drive = '\x0';

  if (m_PathNames.empty())
  {
    //a_Only drive letter found
    m_RelativePath = false;
    return;
  }

  //a_Check if relative or absolute (path starts with /)
  if (m_PathNames.front().isEmpty() || m_Drive != '\x0')
  {
    m_RelativePath = false;
    if (m_Drive == '\x0')
      m_PathNames.pop_front();  //a_Pop front in list since it is empty
  }
  else
    m_RelativePath = true;

  if (m_PathNames.empty())
  {
    //a_No filename
    return;
  }

  //a_Now check if last entry is a filename.ext or a path/
  if (!forceDirectory && '/' != temp.last())
  {
    m_Filename.assign(m_PathNames.back());
    m_PathNames.pop_back();
  }

  //a_Cleanup
  LIST_AString::iterator itKill, it = m_PathNames.begin();
  while (it != m_PathNames.end())
  {
    if ((*it).isEmpty())
    {
      itKill = it;
      ++it;
      m_PathNames.erase(itKill);
    }
    else
    {
      ++it;
    }
  }
}

void AFilename::emit(AOutputBuffer& target) const
{
  emit(target, m_Type);
}

AString AFilename::toAString() const
{
  AString str(MAX_PATH, 256);
  emit(str, m_Type);
  return str;
}


void AFilename::emit(
  AOutputBuffer& target, 
  AFilename::FTYPE ftype, 
  bool noTrailingSlash // = false
) const
{

  //a_Filename
  if (!m_Filename.isEmpty())
  {
    //a_Path
    emitPath(target, ftype);

    //a_Append filename
    target.append(m_Filename);
  }
  else 
  {
    //a_Check if trailing slash is not needed
    emitPath(target, ftype, noTrailingSlash);
  }
}

void AFilename::emitFileUrl(AOutputBuffer& target) const
{
  target.append(FILE_PREFIX);
  emit(target, AFilename::FTYPE_DEFAULT);
}

AXmlElement& AFilename::emitXml(AXmlElement& thisRoot) const
{
  thisRoot.addAttribute(ASW("os", 2), AString::fromInt(m_Type));

  //a_Drive
  if (m_Drive)
    thisRoot.addElement(ASW("drive",5)).addData(m_Drive);

  //a_Path
  AXmlElement& basePath = thisRoot.addElement(ASW("path",4));
  for (LIST_AString::const_iterator cit = m_PathNames.begin(); cit != m_PathNames.end(); ++cit)
  {
    basePath.addElement(ASW("dir",3)).addData(*cit);
  }

  //a_Filename
  if (!m_Filename.isEmpty())
    thisRoot.addElement(ASW("name",4)).addData(m_Filename);

  thisRoot.addElement(ASW("full",4)).addData(*this);
  
  return thisRoot;
}

void AFilename::emitPath(AOutputBuffer& target) const
{
  emitPath(target, m_Type, false);
}

void AFilename::emitPath(AOutputBuffer& target, AFilename::FTYPE ftype, bool noTrailingSlash) const
{
  char sep = '/';
  if (FTYPE_MSDOS == ftype)
    sep = '\\';

  //a_Drive
  if (m_Drive)
  {
    switch (ftype)
    {
      case FTYPE_UNIX: 
      break;

      case FTYPE_CYGWIN:
        target.append(ASW("/cygdrive/",10));
        target.append(m_Drive);
      break;
      
      default:
        if (!m_RelativePath)
        {
          target.append(m_Drive);
          target.append(':');
        }
    }
  }

  //a_Add slash is not a relative path
  if (!m_RelativePath)
    target.append(sep);
  else if (0 == m_PathNames.size())
  {
    //a_Special case, relative and no path, default to . or ./ (depends on noTailingSlash flag)
    target.append(AConstant::ASTRING_PERIOD);
    if (!noTrailingSlash)
      target.append(sep);

    return;
  }

  //a_Path
  LIST_AString::const_iterator cit = m_PathNames.begin();
  while (cit != m_PathNames.end())
  {
    target.append(*cit);
    ++cit;

    if (cit != m_PathNames.end() || !noTrailingSlash)
      target.append(sep);
  }
}

void AFilename::emitNoExt(AOutputBuffer& target) const
{
  emitNoExt(target, m_Type);
}

void AFilename::emitNoExt(AOutputBuffer& target, AFilename::FTYPE ftype) const
{
  emitPath(target, ftype);

  size_t pos = m_Filename.rfind('.');
  if (AConstant::npos != pos)
    m_Filename.peek(target, 0, pos);
  else
    target.append(m_Filename);
}

void AFilename::setType(AFilename::FTYPE t)
{
  m_Type = t;
}

AFilename::FTYPE AFilename::getType() const
{
  return m_Type;
}

void AFilename::clear()
{
  m_Drive = '\x0';
  m_Type = FTYPE_DEFAULT;
  m_PathNames.clear();
  m_Filename.clear();
}

void AFilename::clearFilename()
{
  m_Filename.clear();
}

void AFilename::clearPathNames()
{
  m_PathNames.clear();
}

void AFilename::clearDrive()
{
  m_Drive = '\x0';
}

void AFilename::clearExtension()
{
  setExtension(AConstant::ASTRING_EMPTY);
}

void AFilename::join(
  const AEmittable& that,
  bool forceDirectory
)
{
  AFilename fThat(that, forceDirectory);
  join(fThat);
}

void AFilename::join(const AFilename& that)
{
  if (that.m_Drive && !that.m_RelativePath)
    m_Drive = that.m_Drive;

  LIST_AString::const_iterator cit = that.m_PathNames.begin();
  while (cit != that.m_PathNames.end())
  {
    m_PathNames.push_back(*cit);
    ++cit;
  }

  if (!that.m_Filename.isEmpty())
    m_Filename.assign(that.m_Filename);
}

void AFilename::emitDrive(AOutputBuffer& target) const
{
  target.append(m_Drive);
}

const LIST_AString& AFilename::getPathNames() const
{
  return m_PathNames;
}

const AString& AFilename::getFilename() const
{
  return m_Filename;
}

void AFilename::emitPathAndFilename(AOutputBuffer& target) const
{
  emitPath(target);
  target.append(m_Filename);
}

void AFilename::emitPathAndFilenameNoExt(AOutputBuffer& target) const
{
  emitPath(target);
  emitFilenameNoExt(target);
}

void AFilename::emitFilenameNoExt(AOutputBuffer& target) const
{
  size_t pos = m_Filename.rfind('.');
  if (AConstant::npos == pos)
    m_Filename.emit(target);
  else
    m_Filename.peek(target, 0, pos);
}

void AFilename::emitExtension(AOutputBuffer& ext) const
{
  size_t pos = m_Filename.rfind('.');
  if (AConstant::npos != pos)
    (m_Filename.peek(ext, pos+1) > 0);
}

bool AFilename::isDirectory() const
{
  return m_Filename.isEmpty();
}

bool AFilename::isRelativePath() const
{
  return m_RelativePath;
}

bool AFilename::hasExtension() const
{
  return (AConstant::npos != m_Filename.rfind('.'));
}

bool AFilename::equalsExtension(const AString& ext) const
{
  if (AConstant::npos != m_Filename.rfind('.'))
  {
    AString str(16,16);
    emitExtension(str);
    return (str.equalsNoCase(ext));
  }
  else
    return false;
}

bool AFilename::equalsExtension(const AString& ext1, const AString& ext2) const
{
  //a_Check for filename.ext1.ext2
  AString str(64,16);
  str.assign('.');
  str.append(ext1);
  str.append('.');
  str.append(ext2);
  size_t pos = getFilename().rfind(str);
  if (AConstant::npos != pos && pos == getFilename().getSize() - str.getSize())
    return true;
  else
    return false;
}

char& AFilename::useDrive()
{
  return m_Drive;
}

LIST_AString& AFilename::usePathNames()
{
  return m_PathNames;
}

AString& AFilename::useFilename()
{
  return m_Filename;
}

void AFilename::setExtension(const AString& ext)
{
  size_t pos = m_Filename.rfind('.');
  if (AConstant::npos != pos)
  {
    m_Filename.setSize(pos);
  }

  //a_Set extension if any, else clear extension
  if (!ext.isEmpty())
  {
    m_Filename.append('.');
    m_Filename.append(ext);
  }
}

bool AFilename::operator <(const AFilename& that) const
{
  if (m_Drive < that.m_Drive)
  {
    return true;
  }

  if (m_PathNames.size() > 0)
  {
    //a_Equal @ of path name entries, do compare
    LIST_AString::const_iterator citThis = m_PathNames.begin();
    LIST_AString::const_iterator citThat = that.m_PathNames.begin();
    while(citThis != m_PathNames.end())
    {
      if (citThat == that.m_PathNames.end())
      {
        //a_That is shorter
        return false;
      }

      if (*citThis != *citThat)
      {
        bool ret = (*citThis < *citThat);
        return ret;
      }

      ++citThis;
      ++citThat;
    }
    if (citThat != that.m_PathNames.end())
    {
      //a_This is shorter
      return true;
    }
  }
  else if (that.m_PathNames.size() > 0)
    return true;

  //a_Equal paths at this point leaves it to the filename
  bool ret = (m_Filename < that.m_Filename);
  return ret;
}

bool AFilename::operator ==(const AFilename& that) const
{
  if (m_Drive != that.m_Drive)
    return false;

  if (m_PathNames.size() != that.m_PathNames.size())
    return false;

  LIST_AString::const_iterator citThis = m_PathNames.begin();
  LIST_AString::const_iterator citThat = that.m_PathNames.begin();
  while(citThis != m_PathNames.end())
  {
    if (*citThis != *citThat)
      return false;

    ++citThis;
    ++citThat;
  }

  //a_Equal paths at this point leaves it to the filename
  return (m_Filename == that.m_Filename);
}

bool AFilename::isValid() const
{
  if (AConstant::npos != m_Filename.findOneOf(RESERVED))
    return false;

  if ('\x0' != m_Drive && !isalnum((const u1)m_Drive))
    return false;

  LIST_AString::const_iterator cit = m_PathNames.begin();
  while(cit != m_PathNames.end())
  {
    if (AConstant::npos != (*cit).findOneOf(RESERVED))
      return false;

    ++cit;
  }

  return true;
}

void AFilename::removeBasePath(
  const AFilename& base, 
  bool makeResultAbsolute // = false
)
{
  //a_Verify that base is actually in this
  LIST_AString::iterator it = m_PathNames.begin();
  for (LIST_AString::const_iterator cit = base.m_PathNames.begin(); cit != base.m_PathNames.end(); ++cit, ++it)
  {
    if (it == m_PathNames.end() || !(*it).equals(*cit))
      ATHROW_EX(this, AException::InvalidData, *cit);
  }
  
  for (size_t i = base.m_PathNames.size(); i > 0; --i)
    m_PathNames.pop_front();

  m_RelativePath = !makeResultAbsolute;
}

void AFilename::removeExtension(int count)
{
  for (int i=0; i<count; ++i)
  {
    m_Filename.rremoveUntil('.');
  }
}

bool AFilename::compactPath()
{
  LIST_AString pathNames(m_PathNames.begin(), m_PathNames.end());
  m_PathNames.clear();
  
  LIST_AString::iterator it = pathNames.begin();
  if (it != pathNames.end())
  {
    //a_Iterate and compact as needed
    while(it != pathNames.end())
    {
      if (it->equals(AConstant::ASTRING_PERIOD))
      {
        ++it;
        continue;
      }
      else if (it->equals(AConstant::ASTRING_DOUBLEPERIOD))
      {
        if (
          (
            m_PathNames.size() == 1 
            && !m_PathNames.back().equals(AConstant::ASTRING_DOUBLEPERIOD)
          )
          ||
          (
            m_PathNames.size() > 1 
            && !m_PathNames.back().equals(AConstant::ASTRING_PERIOD)
            && !m_PathNames.back().equals(AConstant::ASTRING_DOUBLEPERIOD)
          )
        )
        {
          if (m_PathNames.back().equals(AConstant::ASTRING_PERIOD))
            m_PathNames.back().assign(AConstant::ASTRING_DOUBLEPERIOD);
          else
            m_PathNames.pop_back();
        }
        else
          m_PathNames.push_back(*it);
      }
      else
        m_PathNames.push_back(*it);

      ++it;
    }
  }

  if (m_PathNames.size() > 0)
    return (!m_PathNames.front().equals(AConstant::ASTRING_DOUBLEPERIOD));
  else
    return true;  // No path means its not relative
}

void AFilename::makeAbsolute()
{
  // Remove leading . or ..
  if (
       m_PathNames.size() > 0
    && 
      (
           m_PathNames.front().equals(AConstant::ASTRING_DOUBLEPERIOD)
        || m_PathNames.front().equals(AConstant::ASTRING_PERIOD)
      )
  )
  {
    m_PathNames.pop_front();
  }

  m_RelativePath = false;
}

void AFilename::makeRelative()
{
  if (
       m_PathNames.size() > 0
    && !m_PathNames.front().equals(AConstant::ASTRING_DOUBLEPERIOD)
    && !m_PathNames.front().equals(AConstant::ASTRING_PERIOD)
  )
  {
    m_PathNames.push_front(AConstant::ASTRING_PERIOD);
  }

  m_RelativePath = true;
}
