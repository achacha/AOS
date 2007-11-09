
#include "pchABase.hpp"
#include "AFileSystem.hpp"
#include <io.h>
#include "AFilename.hpp"
#include "ASystemException.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"

u4 AFileSystem::dir(
  const AFilename& path,
  LIST_AFilename& target,
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
  LIST_AFilename& target,
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
    if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && recurse)
    {
      if (!filesOnly)
      {
        //a_Add directory name
        newpath.usePathNames().push_back(findData.cFileName);
        newpath.clearFilename();
        target.push_back(newpath);
        ++ret;
      }
      
      //a_Recurse
      AFilename newoffsetpath(offsetpath);
      newoffsetpath.usePathNames().push_back(findData.cFileName);
      ret += _getFilesFromPath(path, newoffsetpath, target, true, filesOnly);
    }
    else
    {
      //a_Add file
      newpath.useFilename().assign(findData.cFileName);
      target.push_back(newpath);
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
  if (::rename(from.toAString().c_str(), to.toAString().c_str()))
    ATHROW_LAST_OS_ERROR(NULL);
}

void AFileSystem::remove(const AFilename& source)
{
  if (::remove(source.toAString().c_str()))
    ATHROW_LAST_OS_ERROR(NULL);
}

bool AFileSystem::isA(const AFilename& source, AFileSystem::PATH_TYPE ptype)
{
#ifdef __WINDOWS__
  AString str;
  AFilename::FTYPE ftype = source.getType();
  source.emit(str, AFilename::FTYPE_WIN32, true);
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(str.c_str(), &findData);
  if (INVALID_HANDLE_VALUE == hFind)
    return false;  //a_Does not exist, thus not anything

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
#pragma message("NON-Win32: AFileSystem::isDirectory")
#endif
}

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
  AFilename::FTYPE ftype = source.getType();
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
  AFilename::FTYPE ftype = source.getType();
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
  AFilename::FTYPE ftype = source.getType();
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
  AFilename::FTYPE ftype = source.getType();
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
