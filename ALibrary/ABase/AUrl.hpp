/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AUrl_HPP__
#define INCLUDED__AUrl_HPP__

#include "apiABase.hpp"
#include "AQueryString.hpp"
#include "ANameValuePair.hpp"
#include "ADebugDumpable.hpp"
#include "ASerializable.hpp"
#include "AXmlEmittable.hpp"

/*
protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext#fragment?query

Path separator: /

Path MUST end with a / otherwise last part is assumed to be a name:
  /path0/path1/ ->  path0 and path1 are part of path
  /path0/path1  ->  only path0 is part of part, path1 is a name
*/

class ABASE_API AUrl : public AXmlEmittable, public ADebugDumpable, public ASerializable
{
public:

  /*!
  Default port numbers from RFC-1738 (and more)
  */
  enum eProtocol
  {
    FTP_DATA = 20,   //!< Data port for FTP is 20
    FTP      = 21,   //!< FTP protocol port
    TELNET   = 23,   //!< Telnet port
    SMTP     = 25,   //!< Mailto: may need to go here if so implemented
    GOPHER   = 70,   //!< Gopher, yet another forgotten rodent
    HTTP     = 80,   //!< This is the biscuit of this chicken stand
    NNTP     = 119,  //!< Newsgroup protocol (Usenet, etc)
    WAIS     = 210,  //!< WAIS proxy
    HTTPS    = 443,  //!< This is the secret reclusive friend of the biscuit
    PROSPERO = 1525, //!< Prospero directory service
    MYSQL    = 3306, //!< MySQL
    FILE     = -1,   //!< Flag that can denote that local file access is needed
    INVALID  = -2,   //!< Unintialized
    NONE     = -3,   //!< No port associated with this URL
    ODBC     = -4,   //!< Custom ODBC
    SQLITE   = -5,   //!< Custom SQLite
    DATA     = -6    //!< Data URL as defined by RFC-2397
  };

  //! Path separator
  static const AString sstr_pathSeparator;      //!< "/"
  //! Path to self separator
  static const AString sstr_pathSelf;           //!< "/./"
  //! Path to parent separator
  static const AString sstr_pathParent;         //!< "/../"
  
public:
  //! ctor
  AUrl();
  
  /*!
  Create new URL object
  
  @param url string to parse
  */
  AUrl(const AString& url);
  
  /*!
  Copy ctor

  @param that other object
  */
  AUrl(const AUrl& that);

  //! dtor
  virtual ~AUrl();

  /*!
  Copy operator

  @param that other object
  @return *this
  */
  const AUrl &operator =(const AUrl& that);
  
  /*!
  Will overlay URLs, leaving this data alone if not empty

  @param that other object
  */
  void overlay(const AUrl &that);

  /*!
  Will overlay URLs, leaving this data alone if not empty
  Calls overlay()

  @param that other object
  @return *this
  @see overlay
  */
  const AUrl &operator |=(const AUrl& that);

  /*!
  Will merge URLs, leaving this data alone if not empty

  @param that other object
  @return *this
  */
  AUrl operator |(const AUrl& that) const;

  /*!
  Will merge URLs, will overlay this data with source if source is not empty

  @param that other object
  @return *this
  */
  const AUrl &operator &=(const AUrl& that);

  /*!
  Will merge URLs, will overlay this data with source if source is not empty

  @param that other object
  @return *this
  */
  AUrl operator &(const AUrl& that) const;

  /*!
  Parse the url string

  @param url string to parse
  */
  void parse(const AString& url);
  
  /*!
  Clear
  */
  void clear();

  /*!
  Get password

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'password'
  \endverbatim

  @return constant reference to protocol
  */
  const AString& getProtocol() const;

  /*!
  Get protocol

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return enum for 'protocol:'
  \endverbatim

  @return protocol enumeration
  */
  eProtocol getProtocolEnum() const;

  /*!
  Get username

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'username'
  \endverbatim

  @return constant reference to username
  */
  const AString& getUsername() const;
  
  /*!
  Get password

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'password'
  \endverbatim

  @return constant reference to password
  */
  const AString& getPassword() const;

  /*!
  Get server

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'server'
  \endverbatim

  @return constant reference to server
  */
  const AString& getServer() const;

  /*!
  Get port

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return port number
  \endverbatim
  
  @return port int
  */
  int getPort() const;
  
  /*!
  Get reference to path

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return '/basedir/somedir/otherdir/'
  \endverbatim

  @return constant reference to the path
  */
  const AString& getPath() const;
  
  /*!
  Get base directory

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'basedir'
  \endverbatim
  
  @return string copy of the base directory
  */
  AString getBaseDirName() const;

  /*!
  Get filename

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'filename.ext'
  \endverbatim
  
  @return constant reference to filename
  */
  const AString& getFilename() const;

  /*!
  Get fragment

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext#fragment will return 'fragment'
  \endverbatim
  
  @return constant reference to the fragment
  */
  const AString& getFragment() const;

  /*!
  Get filename without extension

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'filename'
  \endverbatim
  
  @return copy of filename without extension
  */
  AString getFilenameNoExt()const;

  /*!
  Get extension

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'ext'
  \endverbatim
  
  @return copy of extension
  */
  AString getExtension() const;

  /*!
  Get query string

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return 'query'
  \endverbatim
  
  @return copy of query string
  */
  AString getQueryString() const;

  /*!
  Get path and filename

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return '/basedir/somedir/otherdir/filename.ext'
  \endverbatim
  
  @return copy path and filename
  */
  AString getPathAndFilename() const; 

  /*!
  Get path and filename without extension

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return '/basedir/somedir/otherdir/filename'
  \endverbatim
  
  @return copy path and filename without extension
  */
  AString getPathAndFilenameNoExt() const; 

  /*!
  Get path, filename and query string

  \verbatim
  protocol://username:password@server:port/basedir/somedir/otherdir/filename.ext?query will return '/basedir/somedir/otherdir/filename.ext?query'
  \endverbatim
  
  @return copy path, filename and query string
  */
  AString getPathFileAndQueryString() const;

  /*!
  Test if protocol matches
  
  @param e enumerated protocol to test
  @return true if protocol matched
  */
  bool isProtocol(AUrl::eProtocol e) const;

  /*!
  Test if extension matched

  @param ext extension to check without '.'
  @return true if extensions match
  */
  bool isExtension(const AString& ext) const;
 
  /*!
  Check basic validity

  If any character not in AConstant::CHARSET_URL_RFC2396 then it fails

  @return true if the URL doesn't contain invalid characters
  */
  bool isValid() const;

  /*!
  Set new protocol

  @param e enumeration of the protocol
  */
  void setProtocol(eProtocol e);
  
  /*!
  Set protocol

  @param protocol to set
  */
  void setProtocol(const AString& protocol);
  
  /*!
  Set username

  @param username to set
  */
  void setUsername(const AString& username);
  
  /*!
  Set password

  @param password to set
  */
  void setPassword(const AString& password);
  
  /*!
  Set server

  @param server to set
  */
  void setServer(const AString& server);
  
  /*!
  Set port

  @param port to set
  */
  void setPort(int port);
  
  /*!
  Set path

  @param path to set
  */
  void setPath(const AString& path);
  
  /*!
  Set filename

  @param filename to set
  */
  void setFilename(const AString& filename);
  
  /*!
  Set fragment

  @param fragment to set
  */
  void setFragment(const AString& fragment);
  
  /*!
  Set extension

  @param ext extension to set
  */
  void setExtension(const AString& ext);
  
  /*!
  Set filename part only, leave extension as is

  @param filename to set
  */
  void setFilenameNoExt(const AString& filename);
  
  /*!
  Set path and filename

  @param path and filename to set
  */
  void setPathAndFilename(const AString& path);

  /*!
  Support for data: protocol specifics (RFC-2397)

       dataurl    := "data:" [ mediatype ] [ ";base64" ] "," data
       mediatype  := [ type "/" subtype ] *( ";" parameter )
       data       := *urlchar
       parameter  := attribute "=" value

  @return data is URL encoded be default and base64 encoded if so flagged
  */
  AString& useData();
  
  /*!
  data: protocol media type

  @return media type
  @see useData for summary
  */
  AString& useMediaType();
  
  /*!
  data: protocol data encoding
  
  @return true if data is base64 encoded
  @see useData for summary
  */
  bool isBase64Encoded() const;

  /*!
  data: protocol set data encoding
  
  @param base64enc if true then data is base64 encoded
  @see useData for summary
  */
  void setIsBase64Encoded(bool base64enc);

  /*!
  Parameter pair (aka query string) access
  Query string for most protocol (i.e. name=value&name=value&...)
  parameter list in data: protocol (i.e. name=value;name=value;...)

  @return query string
  */
  AQueryString &useParameterPairs();
  
  /*!
  Parameter pair (aka query string) access
  Query string for most protocol (i.e. name=value&name=value&...)
  parameter list in data: protocol (i.e. name=value;name=value;...)

  @return constant reference to query string
  */
  const AQueryString &getParameterPairs() const;

  /*!
  Displays URL (full includes the username:password\@server:port)
    URL generator (Full URL includes username:password)
    boolHidePassword will replace password with *
  
  @param target to append to
  @param showFull if true everything will be shown including username and hidden (shown as "*") for password
  @param hidePassword if true and if showFull is true then actual password will be included
  */
  void emit(AOutputBuffer& target, bool showFull, bool hidePassword = false) const;

  /*!
  AEmittable

  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot to append element to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;
                                           
  /*!
  Return as AString, calls emit internally
  emit() is more efficient, this is more of a convenience when used in ctor init
  
  @return copy of URL string
  */
  AString toAString() const;

  /*!
  Access to last parsing error message
  
  @return constant reference to the last error if any
  */
  const AString& getError() const;

  /*!
  Return a result of a comparison to an object of the same type
  
  @param that other object
  @return 0 if equal, else number of different components
  */
  int compare(const AUrl& that) const;

	/*!
  ASerializable

  @param aFile to write to
  */
  virtual void toAFile(AFile& aFile) const;

	/*!
  ASerializable

  @param aFile to read from
  */
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! log the error
  void _logError(const AString &strError);

  //! Setting methods
  void __setProtocol(const AString &strProtocol);

  //! URL components
  AString m_strProtocol;
  AString m_strUsername;
  AString m_strPassword;
  AString m_strServer;    //!< Server name or MediaType if data: protocol
  int     m_iPort;
  AString m_strPath;
  AString m_strFilename;  //!< Raw data for data: will be stored here, will be encoded as need on emit
  AString m_strFragment;

  //! Support for data: protocol
  bool m_isBase64;

  //! Parameter pairs
  AQueryString m_QueryString;

private:    
  // Parse data protocol
  void _parseDataProtocol(const AString &strInput, size_t pos);

  // Parsing parameters
  void _parseQueryString(const AString &strQueryString);
  void _parsePath(const AString &strPath);

  // data: protocol emit
  void _emitDataProtocol(AOutputBuffer& result) const;

  // Last parsing error
  AString m_strError;
};


#endif
