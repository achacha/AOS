/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AFilename_HPP__
#define INCLUDED__AFilename_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"

// Container of filenames
class AFilename;
typedef std::list<AFilename> LIST_AFilename;

/*!
Wrapper for a full file path
A pure path MUST end in a path delimeter / or \  (e.g. "/some/path/",  "/some/path" will make 'path' a filename)
Multiple delimeters are ignored (e.g. "/some///path//" -> "/some/path/"
*/
class ABASE_API AFilename : public ADebugDumpable, public AXmlEmittable
{
public:
  enum FTYPE
  {
    FTYPE_DEFAULT = 0,  //!< path separated by '/', drive letter included if any
    FTYPE_WIN32 = 0,    //!< default
    FTYPE_UNIX = 1,     //!< path separated by '/', drive letter ignored
    FTYPE_MSDOS = 2,    //!< path separated by '\' (old style, WinNT or better cmd.exe supports '/')
    FTYPE_CYGWIN = 3    //!< separated by '/', drive letter replaced with /cygdrive/X/
  };

  /*! Characters reserved by Windows and invalid in a filename */
  static const AString RESERVED;  //!< Set of reserved filename characters:  |\?*<":>/

public:
  /*!
  ctors
  */
  AFilename();

  /*!
  Create and join path fragments together

  @param filepath base filepath
  */
  AFilename(const AFilename& filepath);

  /*!
  Create and join path fragments together

  @param filepath base filepath
  @param forceDirectory will force all parts to be treated as directories
  */
  AFilename(const AEmittable& filepath, bool forceDirectory);
  
  /*!
  Create and join path fragments together

  @param filepath0 base filepath
  @param filepath1 first joined path
  @param forceDirectory will force all parts to be treated as directories
  */
  AFilename(const AEmittable& filepath0, const AEmittable& filepath1, bool forceDirectory);

  /*!
  Create and join path fragments together

  @param filepath0 base filepath
  @param filepath1 first joined path
  @param filepath2 second joined path
  @param forceDirectory will force all parts to be treated as directories
  */
  AFilename(const AEmittable& filepath0, const AEmittable& filepath1, const AEmittable& filepath2, bool forceDirectory);

  /*!
  Create and join path fragments together

  @param filepath0 base filepath
  @param filepath1 first joined path
  */
  AFilename(const AFilename& filepath0, const AFilename& filepath1);

  /*!
  Create and join path fragments together

  @param filepath0 base filepath
  @param filepath1 first joined path
  @param filepath2 second joined path
  */
  AFilename(const AFilename& filepath0, const AFilename& filepath1, const AFilename& filepath2);

  /*!
  Set to new path, clearing any old data
  forceDirectory forces the path to be a directory regardless of trailing / or not
  */
  void set(const AFilename& filepath);
  void set(const AEmittable& filepath, bool forceDirectory);

  /*!
  Get/Set the type of the filename (determined the output format)
  */
  void setType(AFilename::FTYPE);
  inline AFilename::FTYPE getType() const;

  /*!
  AXmlEmittable

  @param thisRoom to add xml elements to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  AEmittable
  
  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;
  
  /*!
  Special emit to allow forcing a certain type and if trailing slash to be omitted

  @param target to append to
  @param type to filter on
  @param noTrailingSlash appended to directories without filename if true
  */
  void emit(AOutputBuffer& target, AFilename::FTYPE type, bool noTrailingSlash = false) const;

  /*!
  Emits path only

  @param target to emit to
  */
  void emitPath(AOutputBuffer& target) const;

  /*!
  Emits path only

  @param target to emit to
  @param type to use when emitting without changing the current type
  @param noTrailingSlash if the filename is path only
  */
  void emitPath(AOutputBuffer& target, AFilename::FTYPE type, bool noTrailingSlash = false) const;

  /*!
  Emits whole thing without extension

  @param target to emit to
  */
  void emitNoExt(AOutputBuffer& target) const;

  /*!
  Emits whole thing without extension

  @param target to emit to
  @param type to use when emitting without changing the current type
  */
  void emitNoExt(AOutputBuffer& target, AFilename::FTYPE type) const;

  /*!
  Emits path and filename only without extension

  @param target to emit to
  */
  void emitPathAndFilenameNoExt(AOutputBuffer& target) const;

  /*!
  Emits filename only without extension

  @param target to emit to
  */
  void emitFilenameNoExt(AOutputBuffer& target) const;

  /*!
  Emits extension only

  @param target to emit to
  */
  void emitExtension(AOutputBuffer& target) const;

  /*!
  Emits drive only

  @param target to emit to
  */
  void emitDrive(AOutputBuffer& target) const;

  /*!
  Helper
  */
  AString toAString() const;

  /*!
  Overlay AFilename
  that drive will replace this drive
  that path parts will append to this path parts
  that filename will replace this filename

  Works bets when joinging two path fragments
  e.g.
    AFilename f0("c:/basepath/");
    AFilename f1("/some/other/path/and_file");
    f0.join(f1);  // "c:/basepath/some/other/path/and_file"

  e.g.
    AFilename f0("/basepath/");
    AFilename f1("c:/myfile");
    f0.join(f1);  // "c:/basepath/myfile"

  @param that other object to merge with
  */
  void join(const AFilename& that);

  /*!
  Overlay filename
  that drive will replace this drive
  that path parts will append to this path parts
  that filename will replace this filename

  @param that other object to merge with
  @param forceDirectory when filename is not part of that (used when trailing slash may not be present in directory only path)
  */
  void join(const AEmittable& that, bool forceDirectory);

  /*!
  Read only access

  @return constant reference to all path parts
  */
  const LIST_AString& getPathNames() const;

  /*!
  Read only access

  @return constant reference filename only
  */
  const AString& getFilename() const;

  /*!
  Drive if any

  @return reference to drive
  */
  char& useDrive();
  
  /*!
  List of directory names

  @return reference to path part names
  */
  LIST_AString& usePathNames();
  
  /*!
  Filename

  @return reference to filename
  */
  AString& useFilename();

  /*!
  Set extension (no .)

  example:
    f.setExtension("xml");                     // foo.txt -> foo.xml
    f.setExtension(AConstant::ASTRING_EMPTY);  // foo.txt -> foo

  @param ext New extension to set to, if empty extension is removed along with period
  */
  void setExtension(const AString& ext);

  /*!
  Checks if it's a directory

  @return true if this is a pure path without filename
  */
  bool isDirectory() const;
  
  /*!
  Checks if path is relative

  @return true if this is relative path
  */
  bool isRelativePath() const;
  
  /*!
  Checks if filename has an extension

  @return true if extension exists
  */
  bool hasExtension() const;
  
  /*!
  Check if any reserved characters exist

  @return true if no reserved characters are in the path/filename.ext
  */
  bool isValid() const;

  /*!
  Compare extension

  @param ext to compare to
  @return true if extensions match
  */
  bool equalsExtension(const AString& ext) const;

  /*!
  Compare 2 extension:  filename.ext1.ext2

  @param ext1 to compare to
  @param ext2 to compare to
  @return true if extensions match
  */
  bool equalsExtension(const AString& ext1, const AString& ext2) const;

  /*!
  Compact the path removing any /./ and substituting a/../b/ with b/

  @return false if the relative path  is negative, when added to a base path will re-compact below the base (starts with ../)
                i.e. if after compacting something like ../foo results
          true if the first path ofset is not a negative redirect
  */
  bool compactPath();

  /*!
  Removes base path from given path
  this: /foo/bar/baz/filename.ext
  base: /foo/bar
  result if relative=baz/filename.ext
  result if absolute=/baz/filename.ext

  @param base path to remove
  @param makeResultAbsolute if set the result path will be absolute, else relative
  */
  void removeBasePath(const AFilename& base, bool makeResultAbsolute = false);

  /*!
  Remove extension(s)

  @param count of extensions to remove
  */
  void removeExtension(int count = 1);

  /*!
  Operator <
  */
  bool operator <(const AFilename&) const;

  /*!
  Operator ==
  */
  bool operator ==(const AFilename&) const;

  /*!
  Clear all
  */
  void clear();

  /*!
  Clear filename only
  */
  void clearFilename();

  /*!
  Clear all path names
  */
  void clearPathNames();

  /*!
  Clear drive only
  */
  void clearDrive();

  /*!
  Clear extension only
  */
  void clearExtension();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Drive
  char m_Drive;
  
  //! List of path names
  LIST_AString m_PathNames;
  
  //! Filename
  AString m_Filename;
  
  //! Type of a file
  FTYPE m_Type;
  
  //! Is it a relative path
  bool m_RelativePath;
};

#endif // INCLUDED__AFilename_HPP__
