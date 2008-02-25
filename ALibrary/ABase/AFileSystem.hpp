#ifndef INCLUDED__AFileSystem_HPP__
#define INCLUDED__AFileSystem_HPP__

#include "apiABase.hpp"
#include "AFilename.hpp"
#include "ATime.hpp"

class AString;

class ABASE_API AFileSystem
{
public:
  /*!
  Path types used by isA, getType
  */
  enum PathType {
    DoesNotExist = 0x0000,
    Exists       = 0x0001,
    Directory    = 0x0002,
    File         = 0x0004,
    ReadOnly     = 0x0008,
    Compressed   = 0x0010,
    Encrypted    = 0x0020,
    System       = 0x0040,
    Hidden       = 0x0080,
    Temporary    = 0x0100
  };

  class FileInfo : public AXmlEmittable, public ADebugDumpable
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
  };

  typedef std::list<FileInfo> LIST_FileInfo;

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
    AFileSystem::LIST_FileInfo& target,  
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
  DoesNotExist is always 0
  */
  static u4 getType(const AFilename&);
  
  /*!
  Properties
  */
  static bool canRead(const AFilename&);
  static bool canWrite(const AFilename&);
  static bool canReadAndWrite(const AFilename&);

  /*!
  Check if a file exists or make it exist
  */
  static bool exists(const AFilename&);
  static void touch(const AFilename&);

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
  static u4 _getFilesFromPath(const AFilename&, AFilename&, AFileSystem::LIST_FileInfo&, bool, bool);   //a_Recursive internal call

#ifdef __WINDOWS__
  static u4 _winAttrToMask(u4 attr);
#endif
};

#endif //INCLUDED__AFileSystem_HPP__
