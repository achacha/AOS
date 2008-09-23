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

//a_Containers
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
    FTYPE_DEFAULT = 0,  // path separated by '/', drive letter included if any
    FTYPE_WIN32 = 0,    // default
    FTYPE_UNIX = 1,     // path separated by '/', drive letter ignored
    FTYPE_MSDOS = 2,    // path separated by '\' (old style, WinNT or better cmd.exe supports '/')
    FTYPE_CYGWIN = 3    // separated by '/', drive letter replaced with /cygdrive/X/
  };

  /*! Characters reserved by Windows and invalid in a filename */
  static const AString RESERVED;  // |\?*<":>/

public:
  /*!
  ctors
    forceDirectory will force the path to be a directory even if it looks like there is a file at the end (missing trailing '/' et. al.)
  */
  AFilename();
  AFilename(const AFilename& filepath);
  AFilename(const AEmittable& filepath, bool forceDirectory);
  
  /*!
  Create and join
    forceDirectory will force all parts to be treated as directories
  */
  AFilename(const AEmittable& filepath0, const AEmittable& filepath1, bool forceDirectory);
  AFilename(const AEmittable& filepath0, const AEmittable& filepath1, const AEmittable& filepath2, bool forceDirectory);
  AFilename(const AFilename& filepath0, const AFilename& filepath1);
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
  Emits for parts
  */
  void emitPath(AOutputBuffer&) const;
  void emitPath(AOutputBuffer&, AFilename::FTYPE, bool noTrailingSlash = false) const;
  void emitNoExt(AOutputBuffer&) const;
  void emitNoExt(AOutputBuffer&, AFilename::FTYPE) const;
  void emitPathAndFilenameNoExt(AOutputBuffer&) const;
  void emitFilenameNoExt(AOutputBuffer&) const;
  void emitExtension(AOutputBuffer&) const;
  void emitDrive(AOutputBuffer&) const;

  /*!
  Helper
  */
  AString toAString() const;

  /*!
  Overlay AFilename
  that drive will replace this drive
  that path parts will append to this path parts
  that filename will replace this filename
  forceDirectory will force all parts to be treated as directroes

  Works bets when joinging two path fragments
  e.g.
    AFilename f0("c:/basepath/");
    AFilename f1("/some/other/path/and_file");
    f0.join(f1);  // "c:/basepath/some/other/path/and_file"

  e.g.
    AFilename f0("/basepath/");
    AFilename f1("c:/myfile");
    f0.join(f1);  // "c:/basepath/myfile"
  */
  void join(const AEmittable& that, bool forceDirectory);
  void join(const AFilename& that);

  /*!
  Read only access
  */
  const LIST_AString& getPathNames() const;
  const AString& getFilename() const;

  /*!
  Drive if any
  */
  char& useDrive();
  
  /*!
  List of directory names
  */
  LIST_AString& usePathNames();
  
  /*!
  Filename
  */
  AString& useFilename();

  /*!
  Set extension (no .)
    example   f.setExtension("xml");
  */
  void setExtension(const AString&);

  /*!
  Checks if it's a directory
  */
  bool isDirectory() const;
  
  /*!
  Checks if path is relative
  */
  bool isRelativePath() const;
  
  /*!
  Checks if filename has an extension
  */
  bool hasExtension() const;
  
  /*!
  Check if any reserved characters exist
  */
  bool isValid() const;

  /*!
  Compare extension(s)
  */
  bool equalsExtension(const AString&) const;                  //a_Compare to extension
  bool equalsExtension(const AString&, const AString&) const;  //a_2 extensions  filename.ext1.ext2

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

  makeResultAbsolute - if set the result path will be absolute, else relative
  */
  void removeBasePath(const AFilename& base, bool makeResultAbsolute = false);

  /*!
  Remove extension(s)
  */
  void removeExtension(int count = 1);

  /*!
  Operators
  */
  bool operator <(const AFilename&) const;
  bool operator ==(const AFilename&) const;

  /*!
  Clear object (or part of it)
  */
  void clear();
  void clearFilename();
  void clearPathNames();
  void clearDrive();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  char m_Drive;
  LIST_AString m_PathNames;
  AString m_Filename;
  FTYPE m_Type;
  bool m_RelativePath;
};

#endif // INCLUDED__AFilename_HPP__
