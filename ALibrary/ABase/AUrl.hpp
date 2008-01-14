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

  //a_Default port #s from RFC-1738 (and more)
  enum eProtocol
  {
    FTP_DATA = 20,   //a_Data port for FTP is 20
    FTP      = 21,   //a_FTP protocol port
    TELNET   = 23,   //a_Telnet port
    SMTP     = 25,   //a_Mailto: may need to go here if so implemented
    GOPHER   = 70,   //a_Gopher, yet another forgotten rodent
    HTTP     = 80,   //a_This is the biscuit of this chicken stand
    NNTP     = 119,  //a_Newsgroup protocol (Usenet, etc)
    WAIS     = 210,  //a_WAIS proxy
    HTTPS    = 443,  //a_This is the secret reclusive friend of the biscuit
    PROSPERO = 1525, //a_Prospero directory service
    MYSQL    = 3306, //a_MySQL
    FILE     = -1,   //a_Flag that can denote that local file access is needed
    INVALID  = -2,   //a_Unintialized
    NONE     = -3,   //a_No port associated with this URL
    ODBC     = -4,   //a_None
    SQLITE   = -5,   //a_None
    DATA     = -6
  };

  //a_Separators
  static const AString sstr__PathSeparator;      //a_ "/"
  static const AString sstr__PathSelf;           //a_ "/./"
  static const AString sstr__PathParent;         //a_ "/../"
  
public:
  /*!
  ctor/dtor
  */
  AUrl();
  AUrl(const AString &strInput);
  AUrl(const AUrl &urlSource) { operator =(urlSource); }
  virtual ~AUrl();

  /*!
  Copy operator
  */
  const AUrl &operator =(const AUrl &);
  
  /*!
  Will merge URLs, leaving this data alone if not empty
  */
  const AUrl &operator |=(const AUrl &);
  AUrl operator |(const AUrl &) const;

  /*!
  Will merge URLs, will overlay this data with source if source is not empty
  */
  const AUrl &operator &=(const AUrl &);
  AUrl operator &(const AUrl &) const;

  /*!
  Parsing/resetting method
  */
  void parse(const AString &);
  void clear();

  /*!
  Access methods
  */
  const AString& getProtocol() const;
  eProtocol getProtocolEnum() const;
  const AString& getUsername() const;
  const AString& getPassword() const;
  const AString& getServer() const;
  int getPort() const;
  const AString& getPath() const;
  AString getBaseDirName() const;  // protocol://server/basedir/somedir/otherdir/filename.ext will return 'basedir'
  const AString& getFilename() const;
  const AString& getFragment() const;
  AString getFilenameNoExt()const;
  AString getExtension() const;
  AString getParameters() const;
  AString getPathAndFilename() const; 
  AString getPathAndFilenameNoExt() const; 
  AString getPathFileAndQueryString() const;

  /*!
  Tests
  */
  bool isProtocol(AUrl::eProtocol e) const;
  bool isExtension(const AString&) const;
 
  /*!
  Check basic validity
  */
  bool isValid() const;

  /*!
  Modification methods
  */
  void setProtocol(eProtocol);
  void setProtocol(const AString &);
  void setUsername(const AString &);
  void setPassword(const AString &);
  void setServer(const AString &);
  void setPort(int);
  void setPath(const AString &);
  void setFilename(const AString &);
  void setFragment(const AString &);
  void setExtension(const AString &);
  void setFilenameNoExt(const AString &);
  void setPathAndFilename(const AString &);

  /*!
  Support for data: protocol specifics

       dataurl    := "data:" [ mediatype ] [ ";base64" ] "," data
       mediatype  := [ type "/" subtype ] *( ";" parameter )
       data       := *urlchar
       parameter  := attribute "=" value

     data is URL encoded be default and base64 encoded if so flagged
  */
  AString& useMediaType();
  bool isBase64Encoded() const;
  void setIsBase64Encoded(bool);
  AString& useData();

  /*!
  Parameter access
  Query string in most cases and parameter list in data:
  */
        AQueryString &useParameterPairs();
  const AQueryString &getParameterPairs() const;

  /*!
  Displays URL (full includes the u:p@s:p)
    URL generator (Full URL includes username:password)
    boolHidePassword will replace password with *
  */
  void emit(AOutputBuffer&, bool boolFull, bool boolHidePassword = false) const;

  /*!
  AEmittable
  AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  /*!
  Return as AString, calls emit internally
  emit() is more efficient, this is more of a convenience when used in ctor init
  */
  AString toAString() const;

  /*!
  Access to error message (during parse)
  */
  const AString &getError() { return m_strError; }

  /*!
  Return a result of a comparison to an object of the same type
    0 if equal, else number of different componenets
  */
  int compare(const AUrl& aSource) const;

	/*!
  ASerializable
  */
  virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  void _logError(const AString &strError);     //a_Log error

  //a_Setting methods
  void __setProtocol(const AString &strProtocol);

  //a_URL components
  AString m_strProtocol;
  AString m_strUsername;
  AString m_strPassword;
  AString m_strServer;    //a_Server name or MediaType if data: protocol
  int     m_iPort;
  AString m_strPath;
  AString m_strFilename;  //a_Raw data for data: will be stored here, will be encoded as need on emit
  AString m_strFragment;

  //a_Support for data:
  bool m_isBase64;

  //a_Parameters
  AQueryString m_QueryString;

private:    
  //a_Parse data protocol
  void __parseDataProtocol(const AString &strInput, size_t pos);

  //a_Parsing parameters
  void __parseQueryString(const AString &strQueryString);
  void __parsePath(const AString &strPath);

  //a__data: protocol emit
  void __emitDataProtocol(AOutputBuffer& result) const;

  //a_Last error
  AString m_strError;
};


#endif

