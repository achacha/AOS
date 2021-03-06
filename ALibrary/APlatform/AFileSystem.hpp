/*
Written by Alex Chachanashvili

$Id: AFileSystem.hpp 277 2009-01-05 23:11:18Z achacha $
*/
#ifndef INCLUDED__AFileSystem_HPP__
#define INCLUDED__AFileSystem_HPP__

#include "apiAPlatform.hpp"
#include "AFilename.hpp"
#include "ATime.hpp"

class AString;

class APLATFORM_API AFileSystem
{
public:
  /*!
  Path types used by isA, getType
  */
  enum PathType {
    Exists        = 0x0001,    //! Exists
    Directory     = 0x0002,    //! Directory
    File          = 0x0004,    //! File
    ReadOnly      = 0x0008,    //! Read Only
    Compressed    = 0x0010,    //! Compressed
    Encrypted     = 0x0020,    //! Encrypted
    System        = 0x0040,    //! System
    Hidden        = 0x0080,    //! Hidden
    Temporary     = 0x0100,    //! Temporary
    NotAFile      = 0xfffb,    //! ~File
    NotADirectory = 0xfffb,    //! ~Directory
    DoesNotExist  = 0xFFFF
  };

  class APLATFORM_API FileInfo : public AXmlEmittable, public ADebugDumpable
  {
  public:
    FileInfo();
    FileInfo(const FileInfo&);
    AFilename filename;
    s8 length;
    u4 typemask;   // PathType mask

    //! AXmlEmittable
    virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;
    
    //! AEmittable
    virtual void emit(AOutputBuffer&) const;
    
    //! ADebugDumpable
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

    //! Compare operator
    bool operator <(const FileInfo&) const;

    //! Equality operator
    bool operator ==(const FileInfo&) const;

    /*!
    @return true if current item is a directory and not a file
    */
    bool isDirectory() const;

    /*!
    @return true if current item is a file and not a directory
    */
    bool isFile() const;
  };

  typedef std::list<AFileSystem::FileInfo> FileInfos;

public:
  /*!
  Get contents of a directory, ignores /./ and /../
  
  @param basepath Base path for the directory query
  @param target Target list of the AFilename objects (includes basepath)
  @param recurse If true will traverse subdirectories also
  @param filesOnly If true only adds files to the target, else includes directories also
  
  @return # of entries found
  */
  static u4 dir(
    const AFilename& basepath,           
    AFileSystem::FileInfos& target,  
    bool recurse = false,                
    bool filesOnly = true               
  );

  /*!
  Get current working directory
  */
  static void getCWD(AFilename&);

  /*!
  Set current working directory
  */
  static void setCWD(const AFilename&);

  /*!
  Rename
  */
  static void rename(const AFilename& from, const AFilename& to);
  
  /*!
  Remove
  */
  static void remove(const AFilename&);
  
  /*!
  Length of the file, -1 file does not exist
  */
  static s8 length(const AFilename&);

  /*!
  Creation time
  Returns true if file exists and time was set
  */
  static bool getCreatedTime(const AFilename&, ATime&);

  /*!
  Last accessed time
  Returns true if file exists and time was set
  */
  static bool getLastAccessedTime(const AFilename&, ATime&);

  /*!
  Last modified time
  Returns true if file exists and time was set
  */
  static bool getLastModifiedTime(const AFilename&, ATime&);
  
  /*!
  Expand relative to absolute path
  */
  static void expand(const AFilename& relative, AFilename& absolute);

  /*!
  Determines if an existing path is of some type
  */
  static bool isA(const AFilename&, AFileSystem::PathType);

  /*!
  Gets the path type mask of PathType flags
  Useful in checking existance and types in one call
  To check if directory use (AFileSystem::Directory & AFileSystem::getType(...)), etc
  Can return AFileSystem::DoesNotExist
  */
  static u4 getType(const AFilename&);
  
  /*!
  Properties
  */
  static bool canRead(const AFilename&);
  static bool canWrite(const AFilename&);
  static bool canReadAndWrite(const AFilename&);

  /*!
  Check if a file exists (does NOT expand wildcards)

  @param filename to check
  @see existsExpandWildcards
  */
  static bool exists(const AFilename& filename);
  
  /*!
  Check if a file exists expanding wildcards such as ? and *

  @param filename to check
  @see exists
  */
  static bool existsExpandWildcards(const AFilename& filename);

  /*!
  Touch a file (create zero length file if did not exist)

  @param filename to touch
  */
  static void touch(const AFilename& filename);

  /*!
  Create single directory, does not recursively create directories (see method below)

  @param path
  */
  static void createDirectory(const AFilename& path);

  /*!
  Create direciories if missing, only path part of the AFilename is used
  
  @param path to create
  */
  static void createDirectories(const AFilename& path);

  /*!
  Temporary filename
  Given a filepath/filename time based random data will be appended to filename and checked to make sure it does not exist

  @param path base to add filename to
  */
  static void generateTemporaryFilename(AFilename& path);

private:
  static u4 _getFilesFromPath(const AFilename&, AFilename&, AFileSystem::FileInfos&, bool, bool);   //a_Recursive internal call

#ifdef __WINDOWS__
  static u4 _winAttrToMask(u4 attr);
#endif
};

#endif //INCLUDED__AFileSystem_HPP__
