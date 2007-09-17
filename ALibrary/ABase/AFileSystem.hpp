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
  Get contents of a directory, ignores /./ and /../
  returns # of entries found
  */
  static u4 dir(
    const AFilename& basepath,         // Base path for the directory query
    LIST_AFilename& target,            // Target list of the AFilename objects (includes basepath)
    bool recurse = false,              // If true will traverse subdirectories also
    bool filesOnly = true              // If true only adds files to the target, else includes directories also
  );

  /*!
  Get/Set current working directory
  */
  static void getCWD(AFilename&);
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
  enum PATH_TYPE {
    Directory,
    File,
    ReadOnly,
    Compressed,
    Encrypted,
    System,
    Hidden,
    Temporary
  };
  static bool isA(const AFilename&, AFileSystem::PATH_TYPE);

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

private:
  static u4 _getFilesFromPath(const AFilename&, AFilename&, LIST_AFilename&, bool, bool);   //a_Recursive internal call
};

#endif //INCLUDED__AFileSystem_HPP__
