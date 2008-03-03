#ifndef INCLUDED__AFile_Physical_HPP__
#define INCLUDED__AFile_Physical_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "AFile.hpp"
#include "AFilename.hpp"

#include <fcntl.h>       // open flags
#include <share.h>       // share flags
#include <sys/stat.h>    // permission flags

/*!
Wrapper on _fsopen call, see API documentation for open flags and share flags
*/
class ABASE_API AFile_Physical : public AFile
{
public:
  /*!
  Defaults: read only, shareable, reading/writing permitted

  Use "rb" for reading binary files
  */
  static const AString sm_DefaultOpenFlags;          // "rb" binary mode, no translation
  static const int sm_DefaultShareFlags;             // _SH_DENYNO 

  /*!
  Constructs the object but does not open the file
  open() does that
  */
  AFile_Physical();
  AFile_Physical(const AString& filename, const AString& openflags = sm_DefaultOpenFlags, int shareflags = sm_DefaultShareFlags); 
  AFile_Physical(const AFilename& filename, const AString& openflags = sm_DefaultOpenFlags, int shareflags = sm_DefaultShareFlags); 
  virtual ~AFile_Physical();

  /*!
  Open the physical file
  */
  virtual void open();

  /*!
  AFile 
  */
  virtual bool isOpen();
  virtual void flush();
  virtual void close();

  /*!
  AOutputBuffer
  @return Current file size as reported by file system
  */
  virtual size_t getSize() const;

  /*!
  Physical file position helpers
  */
  bool seek(u8 offset, int origin = 0);  // true if seek succeeded
  u8 tell();

  /*!
  Parameter overrides
  Changes apply when open is called
  */
  AFilename& useFilename();
  AString& useOpenFlags();
  int& useShareFlags();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected :
  //a_Implement raw read/write methods that AFile will use
  virtual size_t _write(const void *, size_t);
  virtual size_t _read(void *, size_t);
  virtual bool _isNotEof();

private:
  FILE *mp_file;
  int m_fid;
  
  AFilename m_filename;
  AString m_openflags;
  int m_shareflags;

  AFile_Physical(const AFile_Physical &) {};
  AFile_Physical &operator=(const AFile_Physical &) { return *this; };
};

#endif // INCLUDED__AFile_Physical_HPP__

/*
Documentation for _fsopen from MSDN:

The _fsopen function opens the file specified by filename as a stream and prepares the file for subsequent shared reading or writing, as defined by the mode and shflag arguments. _wfsopen is a wide-character version of _fsopen; the filename and mode arguments to _wfsopen are wide-character strings. _wfsopen and _fsopen behave identically otherwise.

Generic-Text Routine Mappings

TCHAR.H routine  _UNICODE & _MBCS not defined _MBCS defined _UNICODE defined 
_tfsopen _fsopen _fsopen _wfsopen 

The character string mode specifies the type of access requested for the file, as follows: 

"r" 
Opens for reading. If the file does not exist or cannot be found, the _fsopen call fails. 
"w" 
Opens an empty file for writing. If the given file exists, its contents are destroyed. 
"a" 
Opens for writing at the end of the file (appending); creates the file first if it does not exist. 
"r+" 
Opens for both reading and writing. (The file must exist.) 
"w+" 
Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed. 
"a+" 
Opens for reading and appending; creates the file first if it does not exist. 
Use the "w" and "w+" types with care, as they can destroy existing files.

When a file is opened with the "a" or "a+" access type, all write operations occur at the end of the file. The file pointer can be repositioned using fseek or rewind, but is always moved back to the end of the file before any write operation is carried out. Thus existing data cannot be overwritten. When the "r+", "w+", or "a+" access type is specified, both reading and writing are allowed (the file is said to be open for "update"). However, when switching between reading and writing, there must be an intervening fsetpos, fseek, or rewind operation. The current position can be specified for the fsetpos or fseek operation, if desired. In addition to the above values, one of the following characters can be included in mode to specify the translation mode for new lines: 

t 
Opens a file in text (translated) mode. In this mode, carriage return–linefeed (CR-LF) combinations are translated into single linefeeds (LF) on input and LF characters are translated to CR-LF combinations on output. Also, CTRL+Z is interpreted as an end-of-file character on input. In files opened for reading or reading/writing, _fsopen checks for a CTRL+Z at the end of the file and removes it, if possible. This is done because using fseek and ftell to move within a file that ends with a CTRL+Z may cause fseek to behave improperly near the end of the file. 
b 
Opens a file in binary (untranslated) mode; the above translations are suppressed. 
If t or b is not given in mode, the translation mode is defined by the default-mode variable _fmode. If t or b is prefixed to the argument, the function fails and returns NULL. For a discussion of text and binary modes, see Text and Binary Mode File I/O. 

The argument shflag is a constant expression consisting of one of the following manifest constants, defined in SHARE.H: 

_SH_COMPAT 
Sets Compatibility mode for 16-bit applications. 
_SH_DENYNO 
Permits read and write access. 
_SH_DENYRD 
Denies read access to file. 
_SH_DENYRW 
Denies read and write access to file. 
_SH_DENYWR 
Denies write access to file. 
*/