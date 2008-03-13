
#include "pchABase.hpp"
#include "AFileSystem.hpp"
#include <io.h>
#include <errno.h>
#include "AFilename.hpp"
#include "ASystemException.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AXmlElement.hpp"

AFileSystem::FileInfo::FileInfo() :
  length(0),
  typemask(AFileSystem::DoesNotExist)
{
}

AFileSystem::FileInfo::FileInfo(const FileInfo& that) :
  filename(that.filename),
  length(that.length),
  typemask(that.typemask)
{
}

void AFileSystem::FileInfo::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "filename=";
  filename.debugDump(os, indent+2);
  
  ADebugDumpable::indent(os, indent+1) << "length=" << length << std::endl;
  ADebugDumpable::indent(os, indent+1) << "typemask=" << typemask << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AFileSystem::FileInfo::emit(AOutputBuffer& target) const
{
  target.append("mask[",5);
  target.append(AString::fromS4(typemask));
  target.append("] len(",6);
  target.append(AString::fromS8(length));
  target.append("):",2);
  target.append(filename);
}

AXmlElement& AFileSystem::FileInfo::emitXml(AXmlElement& thisRoot) const
{
  if (typemask & AFileSystem::Directory)
    thisRoot.useAttributes().insert(ASW("dir",3), AConstant::ASTRING_ONE);
  else
    thisRoot.useAttributes().insert(ASW("file",4), AConstant::ASTRING_ONE);

  if (typemask & AFileSystem::ReadOnly) thisRoot.useAttributes().insert(ASW("read-only",9), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::Compressed) thisRoot.useAttributes().insert(ASW("compressed",10), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::Encrypted) thisRoot.useAttributes().insert(ASW("encrypted",9), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::System) thisRoot.useAttributes().insert(ASW("system",6), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::Hidden) thisRoot.useAttributes().insert(ASW("hidden",6), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::Temporary) thisRoot.useAttributes().insert(ASW("temporary",9), AConstant::ASTRING_TRUE);
  if (typemask & AFileSystem::DoesNotExist) thisRoot.useAttributes().insert(ASW("exist",5), AConstant::ASTRING_FALSE);

  thisRoot.useAttributes().insert(ASW("typemask",8), AString::fromS4(typemask, 16));
  thisRoot.useAttributes().insert(ASW("length",6), AString::fromS8(length));
  filename.emitXml(thisRoot.addElement(ASW("filename",8)));

  return thisRoot;
}

u4 AFileSystem::dir(
  const AFilename& path,
  AFileSystem::LIST_FileInfo& target,
  bool recurse,   // = false
  bool filesOnly  // = false
)
{
  AFilename offsetpath;
  return _getFilesFromPath(path, offsetpath, target, recurse, filesOnly);
}

u4 AFileSystem::_getFilesFromPath(
  const AFilename& path,
  AFilename& offsetpath,
  AFileSystem::LIST_FileInfo& target,
  bool recurse,   // = false
  bool filesOnly  // = false
)
{
  AString str;
  AFilename newpath(path);
  newpath.join(offsetpath);

  //a_Add wildcard only if filename is not specified
  if (path.isDirectory() && path.getFilename().isEmpty())
  {
  #ifdef __WINDOWS__
    newpath.useFilename().assign("*.*");
  #else
    newpath.useFilename().assign("*");
  #endif
  }
  newpath.emit(str);

#ifdef __WINDOWS__
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    ATHROW_LAST_OS_ERROR_EX(NULL, (AString("Unable to access/list directory: ")+str).c_str());

  //a_ Ignore /./
  if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !strcmp(findData.cFileName, "."))
    if (!::FindNextFile(hFind, &findData))
      return 0;

  //a_ Ignore /../
  if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !strcmp(findData.cFileName, ".."))
    if (!::FindNextFile(hFind, &findData))
      return 0;

  u4 ret = 0;
  do
  {
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && recurse)
    {
      if (!filesOnly)
      {
        //a_Add directory name
        newpath.usePathNames().push_back(findData.cFileName);
        newpath.clearFilename();

        FileInfo info;
        info.filename.set(newpath, true);
        info.typemask = Exists | _winAttrToMask(findData.dwFileAttributes);

        target.push_back(info);

        ++ret;
      }
      
      //a_Recurse
      AFilename newoffsetpath(offsetpath);
      newoffsetpath.usePathNames().push_back(findData.cFileName);
      ret += _getFilesFromPath(path, newoffsetpath, target, true, filesOnly);
    }
    else
    {
      //a_Add directory name if requested only
      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && filesOnly)
        continue;

      //a_Add file
      newpath.join(ASWNL(findData.cFileName), false);

      FileInfo info;
      info.filename.set(newpath, false);
      info.length = findData.nFileSizeHigh;
      info.length <<= 32;
      info.length |= findData.nFileSizeLow;
      info.typemask = Exists | _winAttrToMask(findData.dwFileAttributes);

      target.push_back(info);
      
      ++ret;
    }
  }
  while(::FindNextFile(hFind, &findData));

  ::FindClose(hFind);
  return ret;
#else
#error "NON-Win32: AFileSystem::dir"
#endif
}

bool AFileSystem::exists(const AFilename& source)
{
  return (!_access(source.toAString().c_str(), 0) ? true : false);
}

void AFileSystem::getCWD(AFilename& target)
{
#ifdef __WINDOWS__
  DWORD bufferSize = 2048;
  AString buffer(bufferSize, 256);
  bufferSize = ::GetCurrentDirectory(bufferSize, buffer.startUsingCharPtr());
  if (!bufferSize)
    ATHROW_LAST_OS_ERROR(NULL);
  
  buffer.stopUsingCharPtr(bufferSize);
  target.set(buffer, true);
#else
#error "NON-Win32: AFileSystem::getCWD"
#endif
}

void AFileSystem::setCWD(const AFilename& source)
{
#ifdef __WINDOWS__
  AVERIFY(NULL, ::SetCurrentDirectory(source.toAString().c_str()));
#else
#error "NON-Win32: AFileSystem::setCWD"
#endif
}

void AFileSystem::rename(const AFilename& from, const AFilename& to)
{
  switch (::rename(from.toAString().c_str(), to.toAString().c_str()))
  {
    case 0:
      return;

    case EACCES:
      ATHROW_EX(&from, AException::APIFailure, ARope("File or directory specified by newname already exists or could not be created (invalid path, access violation); or oldname is a directory and newname specifies a different path: ")+to);

    case ENOENT:
      ATHROW_EX(&from, AException::APIFailure, ASWNL("File not found"));

    case EINVAL:
      ATHROW_EX(&to, AException::APIFailure, ASWNL("Name contains invalid characters"));

    default:  
      ATHROW_LAST_OS_ERROR(NULL);
  }
}

void AFileSystem::remove(const AFilename& source)
{
  switch(::remove(source.toAString().c_str()))
  {
    case 0:
      return;

    case EACCES:
      ATHROW_EX(&source, AException::APIFailure, ASWNL("File speficied is no accessable, read-only or still open"));

    case ENOENT:
      ATHROW_EX(&source, AException::APIFailure, ASWNL("File not found"));

    case EINVAL:
      ATHROW_EX(&source, AException::APIFailure, ASWNL("Name contains invalid characters"));

    default:
      ATHROW_LAST_OS_ERROR(NULL);
  }
}

bool AFileSystem::isA(const AFilename& source, AFileSystem::PathType ptype)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return (ptype == DoesNotExist);   //a_Existance check

  ::FindClose(hFind);
  switch(ptype)
  {
  case Directory:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false);

  case File:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? false : true);

  case ReadOnly:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY ? false : true);

  case Compressed:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED ? true : false);

  case Encrypted:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED ? true : false);

  case System:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ? true : false);

  case Hidden:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ? true : false);

  case Temporary:
    return (findData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ? true : false);

  default:
    ATHROW(NULL, AException::NotSupported);
  }
#else
#pragma message("NON-Win32: AFileSystem::isA")
#endif
}

u4 AFileSystem::getType(const AFilename& source)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return DoesNotExist;   //a_Existance check

  ::FindClose(hFind);
  u4 ret = Exists | _winAttrToMask(findData.dwFileAttributes);
  return ret;
#else
#pragma message("NON-Win32: AFileSystem::isDirectory")
#endif
}

#ifdef __WINDOWS__
u4 AFileSystem::_winAttrToMask(u4 attr)
{
  u4 ret = 0;
  if (attr & FILE_ATTRIBUTE_DIRECTORY)
    ret |= Directory;

  if (attr & FILE_ATTRIBUTE_DIRECTORY)
    ret |= File;

  if (attr & FILE_ATTRIBUTE_READONLY)
    ret |= ReadOnly;

  if (attr & FILE_ATTRIBUTE_COMPRESSED)
    ret |= Compressed;

  if (attr & FILE_ATTRIBUTE_ENCRYPTED)
    ret |= Encrypted;

  if (attr & FILE_ATTRIBUTE_SYSTEM)
    ret |= System;

  if (attr & FILE_ATTRIBUTE_HIDDEN)
    ret |= Hidden;

  if (attr & FILE_ATTRIBUTE_TEMPORARY)
    ret |= Temporary;

  return ret;
}
#endif

bool AFileSystem::canRead(const AFilename& source)
{
  return (!_access(source.toAString().c_str(), 4) ? true : false);
}

bool AFileSystem::canWrite(const AFilename& source)
{
  return (!_access(source.toAString().c_str(), 2) ? true : false);
}

bool AFileSystem::canReadAndWrite(const AFilename& source)
{
  return (!_access(source.toAString().c_str(), 6) ? true : false);
}

void AFileSystem::expand(const AFilename& relative, AFilename& absolute)
{
  size_t bufferSize = 2048;
  AString buffer(bufferSize, 256);
  if (!_fullpath(buffer.startUsingCharPtr(), relative.toAString().c_str(), bufferSize))
    ATHROW_LAST_OS_ERROR(NULL);
  
  buffer.stopUsingCharPtr();
  absolute.set(buffer, false);
}

void AFileSystem::touch(const AFilename& source)
{
  static const AString OPENFLAGS("w",1);
  AFile_Physical f(source.toAString(), OPENFLAGS);
  f.open();
  f.close();
}

s8 AFileSystem::length(const AFilename& source)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return -1;

  ::FindClose(hFind);

  AASSERT(NULL, !(0x8000 & findData.nFileSizeHigh));  //a_Very big file... gotta fix this code
  s8 ret = findData.nFileSizeHigh;
  ret <<= 32;
  ret |= findData.nFileSizeLow;
  return ret;
#else
#pragma error("Not implemented yet")
#endif
}

bool AFileSystem::getCreatedTime(const AFilename& source, ATime& ftime)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return false;

  ::FindClose(hFind);

  s8 ret = findData.ftCreationTime.dwHighDateTime;
  ret <<= 32;
  ret |= findData.ftCreationTime.dwLowDateTime;

  //a_Microsoft magic conversion from FILEDATE to time_t
  ret -= 116444736000000000L;
  ret /= 10000000L;

  ftime.set((time_t)ret);
  return true;
#else
#pragma error("Not implemented yet")
#endif
}

bool AFileSystem::getLastAccessedTime(const AFilename& source, ATime& ftime)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return false;

  ::FindClose(hFind);

  s8 ret = findData.ftLastAccessTime.dwHighDateTime;
  ret <<= 32;
  ret |= findData.ftLastAccessTime.dwLowDateTime;

  //a_Microsoft magic conversion from FILEDATE to time_t
  ret -= 116444736000000000L;
  ret /= 10000000L;

  ftime.set((time_t)ret);
  return true;
#else
#pragma error("Not implemented yet")
#endif
}

bool AFileSystem::getLastModifiedTime(const AFilename& source, ATime& ftime)
{
#ifdef __WINDOWS__
  AString str;
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return false;

  ::FindClose(hFind);

  s8 ret = findData.ftLastWriteTime.dwHighDateTime;
  ret <<= 32;
  ret |= findData.ftLastWriteTime.dwLowDateTime;

  //a_Microsoft magic conversion from FILEDATE to time_t
  ret -= 116444736000000000L;
  ret /= 10000000L;

  ftime.set((time_t)ret);
  return true;
#else
#pragma error("Not implemented yet")
#endif
}

void AFileSystem::generateTemporaryFilename(AFilename& path)
{
  do
  {
    ATextGenerator::generateUniqueId(path.useFilename());
  }
  while(AFileSystem::exists(path));
}

void AFileSystem::createDirectory(const AFilename& path)
{
#ifdef __WINDOWS__
  AString strPath;
  path.emit(strPath);
  if (!::CreateDirectory(strPath.c_str(), NULL))
    ATHROW_LAST_OS_ERROR(&path);
#else
#pragma error("Not implemented yet")
#endif
}

void AFileSystem::createDirectories(const AFilename& path)
{
  LIST_AString toCreate;
  AFilename testPath(path);
  testPath.clearFilename();
  testPath.compactPath();
  while (!AFileSystem::exists(testPath) && testPath.usePathNames().size())
  {
    toCreate.push_front(testPath.usePathNames().back());
    testPath.usePathNames().pop_back();
  }

  while (toCreate.size())
  {
    testPath.usePathNames().push_back(toCreate.front());
    toCreate.pop_front();
    AFileSystem::createDirectory(testPath);
  }
}
