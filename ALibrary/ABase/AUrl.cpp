/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AUrl.hpp"
#include "AString.hpp"
#include "AOutputBuffer.hpp"
#include "ATextConverter.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"

const AString AUrl::sstr_pathSeparator("/");
const AString AUrl::sstr_pathSelf("/./");
const AString AUrl::sstr_pathParent("/../");

void AUrl::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  //a_URL components
  ADebugDumpable::indent(os, indent+1) << "m_strProtocol=" << m_strProtocol
    << "  m_strUsername=" << m_strUsername
    << "  m_strPassword=" << m_strPassword
    << "  m_strServer=" << m_strServer
    << "  m_iPort=" << m_iPort << std::endl;          
  ADebugDumpable::indent(os, indent+1) << "m_strPath=" << m_strPath
    << "  m_strFilename=" << m_strFilename 
    << "  m_strFragment=" << m_strFragment << std::endl;    
  
  ADebugDumpable::indent(os, indent+1) << "m_QueryString=" << std::endl;    
  m_QueryString.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AUrl::AUrl() :
  m_iPort(AUrl::INVALID)
{
}

AUrl::AUrl(const AString &url)
{
  parse(url);
}

AUrl::AUrl(const AUrl &that)
{ 
  operator =(that);
}


AUrl::~AUrl()
{
}

void AUrl::clear()
{
  //a_URL components
  m_strProtocol.clear();
  m_strUsername.clear();
  m_strPassword.clear();
  m_strServer.clear();
  m_iPort = AUrl::INVALID;
  m_strPath.clear();
  m_strFilename.clear();
  m_strFragment.clear();

  //a_Form elements
  m_QueryString.clear();

  //a_Last error
  m_strError.clear();
}

const AUrl &AUrl::operator =(const AUrl& that)
{
  //a_URL components
  m_strProtocol = that.m_strProtocol;
  m_strUsername = that.m_strUsername;
  m_strPassword = that.m_strPassword;
  m_strServer   = that.m_strServer;
  m_iPort       = that.m_iPort;
  m_strPath     = that.m_strPath;
  m_strFilename = that.m_strFilename;
  m_strFragment = that.m_strFragment;
  m_strError    = that.m_strError;
  m_QueryString = that.m_QueryString;

  return *this;
}

AUrl AUrl::operator |(const AUrl &urlSource) const
{
  AUrl urlReturn(*this);
  urlReturn |= urlSource;

  return urlReturn;
}

const AUrl &AUrl::operator |=(const AUrl &urlSource)
{
  //a_URL components
  if (m_strProtocol.isEmpty()) m_strProtocol = urlSource.m_strProtocol;
  if (m_strServer.isEmpty()) m_strServer = urlSource.m_strServer;
  if (m_iPort == AUrl::INVALID)
  {
    m_strServer = urlSource.m_strServer;
    m_iPort = urlSource.m_iPort;
  }

  //a_Preserve username/password if server name is same, otherwise it may not be of much use
  if (m_strServer == urlSource.m_strServer)
  {
    if (m_strUsername.isEmpty()) m_strUsername = urlSource.m_strUsername;
    if (m_strPassword.isEmpty()) m_strPassword = urlSource.m_strPassword;
  }
  else
  {
    m_strUsername.clear();
    m_strPassword.clear();
  }

  if (m_strPath.isEmpty())
  {
    m_strPath = urlSource.m_strPath;
    
    // Change current filename/query if it is empty only
    if (m_strFilename.isEmpty())
    {
      m_strFilename = urlSource.m_strFilename;
      m_strFragment = urlSource.m_strFragment;
      
      //If target URL didn't have form items, remove local items since we are using target filename
      if (urlSource.m_QueryString.size() == 0)
        m_QueryString.clear();
    }
  }
  else
  {
    if (m_strPath[0] != '/')
    {
      //a_We have some relative path
      if (!urlSource.m_strPath.isEmpty())
      {
        if (urlSource.m_strPath[0] == '/' && m_strPath[0] != '/')
        {
          if (urlSource.m_strPath.at(urlSource.m_strPath.getSize()-1) == '/')
            m_strPath = urlSource.m_strPath + m_strPath;    //a_Add relative local to absolute target
          else
            m_strPath = urlSource.m_strPath + '/' + m_strPath;    //a_Add relative local to absolute target

          //a_Add trailing path
          if (m_strPath.at(m_strPath.getSize()-1) != '/')
            m_strPath.append('/');

          //a_Add to the current form
          if (!m_strFilename.isEmpty())
            m_QueryString |= urlSource.m_QueryString;
        }
        else if (urlSource.m_strPath[0] != '/' && m_strPath[0] == '/')
        {
          m_strPath = m_strPath + urlSource.m_strPath;    //a_Add relative target to absolute local
          m_strFilename = urlSource.m_strFilename;         //a_Keep target filename
          m_strFragment = urlSource.m_strFragment;

          //If target URL didn't have form items, remove local items since we are using target filename
          if (urlSource.m_QueryString.size() == 0)
            m_QueryString.clear();
        }
        else
        {
          m_strPath = urlSource.m_strPath;
          m_strFilename = urlSource.m_strFilename;
          m_strFragment = urlSource.m_strFragment;

          //If target URL didn't have form items, remove local items since we are using target filename
          if (urlSource.m_QueryString.size() == 0)
            m_QueryString.clear();
        }
         
        //a_Contract the redirections like ".." by removing preceding directory
        //a_Find all the "." directories and remove them
        size_t iPos, iCutPos;
        while ((iPos = m_strPath.find(sstr_pathSelf)) != AConstant::npos)
        {
          //a_Remove the "."
          if (m_strPath.getSize() > (iPos + 2))
          {
            AString str;
            m_strPath.peek(str, iPos + 3);
            m_strPath.assign(str);
          }
          else
          {
            AString str;
            m_strPath.peek(str, 0, iPos + 1);
            m_strPath.assign(str);
          }
        }
        while ((iPos = m_strPath.find(sstr_pathParent)) != AConstant::npos)
        {
          //a_Degenerate case of /../ being the first part of the path in root
          if (iPos == 0x0)
          {
            if ((iCutPos = m_strPath.find('/', iPos + 2)) == AConstant::npos)
            {
              //a_Completely invalid path
              ATHROW(this, AException::InvalidPathRedirection);
            }
            else
            {
              //a_Remove /.. since it makes no sense!
              m_strPath.remove(iCutPos);
              continue;
            }
          }
          
          //a_We have "/.." to remove
          //a_EG: /somedir/../somemoredir/ -> /somedir/somemoredir/
          if ((iCutPos = m_strPath.rfind('/', iPos - 1)) != AConstant::npos)
          {
            //a_Found another directory... remove it
            m_strPath.remove(3, iCutPos);
          }
        }
      }
      else
        _logError("Current path is relative and destination is missing");
    }
    else
    {
      //a_Allow appends to a path only (ending with /) and with only a relative path
      if (! urlSource.m_strPath.isEmpty())
      {
        if (urlSource.m_strPath[0] != '/')
        {
          //a_We have a relative source, check if this is a path and allow an append
          if (m_strPath[m_strPath.getSize() - 0x1] == '/')
          {
            m_strPath += urlSource.m_strPath;

            //a_Appended target path, filename is taken from target if any
            m_strFilename = urlSource.m_strFilename;
            m_strFragment = urlSource.m_strFragment;

            //If target URL didn't have form items, remove local items since we are using target filename
            if (urlSource.m_QueryString.size() == 0)
              m_QueryString.clear();
          }
        }
        else
        {
          // Source URL path is empty but may have a filename
          if (! urlSource.m_strFilename.isEmpty())
          {
            m_strFilename = urlSource.m_strFilename;
            m_strFragment = urlSource.m_strFragment;

            //If target URL didn't have form items, remove local items since we are using target filename
            if (urlSource.m_QueryString.size() == 0)
              m_QueryString.clear();
          }
        }
      }
      else
      {
        // Source URL path is empty but may have a filename
        if (! urlSource.m_strFilename.isEmpty())
        {
          m_strFilename = urlSource.m_strFilename;
          m_strFragment = urlSource.m_strFragment;

          //If target URL didn't have form items, remove local items since we are using target filename
          if (urlSource.m_QueryString.size() == 0)
            m_QueryString.clear();
        }
      }
    }
  }
    
  return *this;
}

AUrl AUrl::operator &(const AUrl& that) const
{
  AUrl urlReturn(*this);
  urlReturn &= that;

  return urlReturn;
}

const AUrl &AUrl::operator &=(const AUrl& that)
{
  //a_URL components
  if (!that.m_strProtocol.isEmpty()) 
    m_strProtocol = that.m_strProtocol;
  if (that.m_iPort != AUrl::INVALID) 
    m_iPort = that.m_iPort;
  if (!that.m_strServer.isEmpty()) 
    m_strServer = that.m_strServer;
  if (!that.m_strUsername.isEmpty()) 
    m_strUsername = that.m_strUsername;
  if (!that.m_strPassword.isEmpty()) 
    m_strPassword = that.m_strPassword;
  if (!that.m_strPath.isEmpty())
  {
    m_strPath = that.m_strPath;
    m_strFilename.clear();
    m_strFragment.clear();
  }
  if (!that.m_strFilename.isEmpty())
  { 
    m_strFilename = that.m_strFilename;
    m_strFragment = that.m_strFragment;
  }

  m_QueryString |= that.m_QueryString;

  return *this;
}

void AUrl::setProtocol(eProtocol protocol)
{
  m_iPort = protocol;
  switch(protocol)
  {
    case AUrl::HTTP     : m_strProtocol.assign("http:",5); break;
    case AUrl::HTTPS    : m_strProtocol.assign("https:",6); break;
    case AUrl::FTP      : m_strProtocol.assign("ftp:",4); break;
    case AUrl::FILE     : m_strProtocol.assign("file:",5); break;
    case AUrl::MYSQL    : m_strProtocol.assign("mysql:",6); break;
    case AUrl::ODBC     : m_strProtocol.assign("odbc:",5); break;
    case AUrl::SQLITE   : m_strProtocol.assign("sqlite:",7); break;
    case AUrl::NNTP     : m_strProtocol.assign("news:",5); break;
    case AUrl::SMTP     : m_strProtocol.assign("mailto:",7); break;
    case AUrl::GOPHER   : m_strProtocol.assign("gopher:",7); break;
    case AUrl::TELNET   : m_strProtocol.assign("telnet:",7); break;
    case AUrl::WAIS     : m_strProtocol.assign("wais:",5); break;
    case AUrl::PROSPERO : m_strProtocol.assign("prospero:",9); break;
    default:
      ATHROW(this, AException::NotImplemented);
  }
}

void AUrl::parse(const AString &strInput)
{
  //a_Reset the URL
  clear();
  
  //a_Remove the CGI parameters first
  size_t pos = strInput.find('?');
  AString strURL;
  if (pos != AConstant::npos)
  {
    //a_Found parameters, parse them
    AString str;
    strInput.peek(str, pos);
    _parseQueryString(str);

    strInput.peek(strURL, 0, pos);
  }
  else
    strURL = strInput;

  //a_Extract URL sans parameters
  bool boolServerStartFound = false;
  if ((pos = strURL.find(':')) != AConstant::npos)
  {
    //a_Copy the protocol
    ++pos;
    AString str;
    strURL.peek(str, 0, pos);
    setProtocol(str);

    if (strURL.getSize() > pos + 1)
      if ((strURL[pos] == '/') && (strURL[pos + 1] == '/'))
      {
        pos += 2;
        boolServerStartFound = true;
      }
      else
        _logError("Did not find // after protocol:");
    
    //a_Remove the protocol and "//"
    strURL.remove(pos);
  }

  if (m_strProtocol.equals("data:",5))
  {
    //a_data: protocol
    _parseDataProtocol(strInput, pos);
  }
  else
  {
    //a_Now separate the username:password@remoteserver:port
    if ((pos = strURL.rfind('@')) != AConstant::npos)
    {
      if (pos == 0)
      {
        pos++;
      }
      else
      {
        //a_Extract username:password info
        m_strUsername.clear();
        m_strPassword.clear();

        size_t separator = strURL.find(':');
        if (separator != AConstant::npos)
        {
          //a_We have both name and password
          strURL.peek(m_strUsername, 0, separator);
          strURL.peek(m_strPassword, separator + 1, pos - separator - 1);
        }
        else
        {
          //a_Only username found
          strURL.peek(m_strUsername, 0, pos);
        }
      }

      //a_Adjust the URL by removing the u:p portion
      strURL.remove(pos + 1);
    }

    if (strURL.isEmpty())
      return;

    //a_Now we should have <server>:<port>/<path><filename>?<CGI parameters>
    size_t pathpos = strURL.find('/');
    m_strServer.clear();
    if (pathpos != AConstant::npos)
    {
      //a_We have both name and password up to the path
      size_t separator = strURL.find(':');
      if (separator != AConstant::npos && pathpos > separator)
      {
        //a_Server, port and path
        strURL.peek(m_strServer, 0, separator);

        AString str;
        strURL.peek(str, separator + 1, pathpos - separator);
        m_iPort = atoi(str.c_str());

        //a_Extract copied info
        strURL.remove(pathpos);
      }
      else
      {
        //a_No port but path exists, server is only valid if '//' was found
        //a_ or : is found after /, so it must be in th path/filename/query area
        if (boolServerStartFound)
        {
          strURL.peek(m_strServer, 0, pathpos);

          //a_Extract copied info
          strURL.remove(pathpos);
        }
      }
    }
    else
    {
      //a_We have both name and password but no '/' to denote start of path, we are done
      size_t separator = strURL.find(':');
      if (separator != AConstant::npos)
      {
        //a_Server and port but no path
        strURL.peek(m_strServer, 0, separator);
        AString str;
        strURL.peek(str, separator + 1);
        m_iPort = atoi(str.c_str());

        //a_Force it to be root since no path was specified but a server was
        strURL = '/';  
      }
      else
      {
        //a_No path and no port (then server will follow, special case for mailto)
        if ((boolServerStartFound == true) || (m_strProtocol == "mailto:"))
        {
          strURL.peek(m_strServer, 0, pathpos);

          //a_Force it to be root since no path was specified but a server was
          strURL = '/';  
        }
      }
    }

    if (m_iPort == AUrl::INVALID)
      m_iPort = AUrl::HTTP;  //a_No port specified or found, play it safe with 80

    if (strURL.isEmpty())
      return;

    //a_All we can have now is just the path info
    _parsePath(strURL);
  }

  return;
}

void AUrl::_parseDataProtocol(const AString &strInput, size_t pos)
{
  //a_Format should be <media type>;{name0=value0;name1=value1}{;base64,}<data>
  AString str;
  pos = strInput.peekUntil(str, pos, AConstant::ASTRING_COMMA);
  if (AConstant::npos == pos)
  {
    _logError(ASWNL("data: protocol must have a , to delimit start of data"));
    return;
  }

  //a_Get media type
  size_t i = str.getUntil(m_strServer, AConstant::ASTRING_SEMICOLON);
  if (AConstant::npos == i)
  {
    _logError(ASWNL("data: media type unknown, defaulting to application/octet-stream"));
    m_strServer.assign("application/octet-stream",24);
  }

  //a_Get if ";base64"
  if (
       str.getSize() >= 6 
    && str.getSize() - 6 == str.rfind(ASW("base64",6))
  )
  {
    m_isBase64 = true;
    str.rremove(6);
  }
  else
    m_isBase64 = false;

  //a_Parse parameters
  if (!str.isEmpty())
    m_QueryString.parse(str, ANameValuePair::DATA);

  m_strFilename.clear();
  m_strFragment.clear();
  ATextConverter::decodeBase64(strInput, pos, m_strFilename);
}

void AUrl::_parsePath(const AString &strPath)
{
  //a_Separate the path from filename
  m_strPath.clear();
  m_strFilename.clear();
  m_strFragment.clear();
  
  //a_See if we have "/" case
  if (strPath.getSize() == 1 && strPath.at(0,'\x0') == '/')
  {
    m_strPath.assign('/');
    return;
  }

  //a_Check if relative path is needed
  u4 startofpath = 0;
  if (strPath.at(0,'\x0') == '/' && strPath.at(1,'\x0') == '.')
  {
    ++startofpath;  // a_Use ./ as the start
  }

  size_t endofpath = strPath.rfind('/');
  if (endofpath != AConstant::npos)
  {
    //a_Found separator, filename exists, a path MUST have / at the end of it else it is a filename
    strPath.peek(m_strPath, startofpath, endofpath - startofpath + 1);
    strPath.peek(m_strFilename, endofpath + 1);
  }
  else
  {
    //a_No separator, all filename
    m_strFilename = strPath;
  }

  startofpath = m_strFilename.find('#');
  if (AConstant::npos != startofpath)
  {
    m_strFilename.peek(m_strFragment, startofpath+1);
    m_strFilename.setSize(startofpath);
  }
}

void AUrl::_parseQueryString(const AString &strParams)
{
  m_QueryString.parse(strParams);

  return;
}

AString AUrl::getBaseDirName() const
{
  AString strReturn;

  //a_Extract base dir from path
  if (!m_strPath.isEmpty())
  {
    size_t start = 0;
    if (m_strPath[0] == '/')
      start=1;
    size_t slash = m_strPath.find('/', start);
    if (AConstant::npos != slash)
    {
      if (slash > start)
        m_strPath.peek(strReturn, start, slash-start);
      else
        m_strPath.peek(strReturn, start);
    }
  }

  return strReturn;
}

AString AUrl::getPathFileAndQueryString() const
{
  AString strReturn;
  
  //a_Path
  if (!m_strPath.isEmpty())
  {
    strReturn.append(m_strPath);
  }
  
  //a_Filename
  if (!m_strFilename.isEmpty())
  {
    strReturn.append(m_strFilename);
    if (!m_strFragment.isEmpty())
    {
      strReturn.append('#');
      strReturn.append(m_strFragment);
    }
  }

  //a_Form parameters
  if (m_QueryString.size() > 0)
  {
    strReturn.append('?');
    m_QueryString.emit(strReturn);
  }

  return strReturn;
}

AXmlElement& AUrl::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.addElement(ASW("protocol",8)).addData(m_strProtocol);
  if (m_strProtocol.equals("data:", 5))
  {
    thisRoot.addElement(ASW("mediaType",9)).addData(m_strServer);
    thisRoot.addElement(ASW("base64",6)).addData(m_isBase64 ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE);
    thisRoot.addElement(ASW("data",4)).addData(m_strFilename, AXmlElement::ENC_CDATASAFE);
  }
  else
  {
    thisRoot.addElement(ASW("username",8)).addData(m_strUsername);
    thisRoot.addElement(ASW("password",8)).addData(m_strPassword);
    thisRoot.addElement(ASW("server",6)).addData(m_strServer);
    if (AUrl::NONE != m_iPort)
      thisRoot.addElement(ASW("port",4)).addData(m_iPort);
    thisRoot.addElement(ASW("path",4)).addData(m_strPath);
    thisRoot.addElement(ASW("filename",8)).addData(m_strFilename);
  }

  thisRoot.addElement(ASW("url",3), *this, AXmlElement::ENC_CDATASAFE);

  m_QueryString.emitXml(thisRoot.addElement(ASW("query-string",12)));
  
  return thisRoot;
}

void AUrl::emit(AOutputBuffer& target) const
{
  if (m_strProtocol.equals("data:",5))
  {
    //a_data: protocol emit
    _emitDataProtocol(target);
  }
  else
  {
    emit(target, true);
  }
}

//a_Full URL is default but name:password may not be needed
void AUrl::emit(AOutputBuffer& result, bool boolFull, bool boolHidePassword /* = false */) const
{
  //a_Protocol
  result.append(m_strProtocol);
  if ((!m_strProtocol.isEmpty()) &&          //a_No protocol, no //
      (m_strProtocol != "mailto:"))          //a_Mailto does not need the //
      result.append(AConstant::ASTRING_DOUBLESLASH);

  //a_Username:Password
  if ((boolFull) && (!m_strUsername.isEmpty()))
  {
    result.append(m_strUsername);
    if (!m_strPassword.isEmpty())
    {
      result.append(':');
      if (boolHidePassword)
        result.append('*');
      else
        result.append(m_strPassword);
    }
    result.append('@');
  }

  //a_Server:Port
  result.append(m_strServer);
  if (
    m_iPort > 0
    && !(m_iPort == AUrl::HTTP && m_strProtocol.equals("http:",5))
    && !(m_iPort == AUrl::HTTPS && m_strProtocol.equals("https:",6))
    && !m_strServer.isEmpty()
  )
  {
    result.append(':');
    result.append(AString::fromInt(m_iPort));
  }

  //a_Remained is the HTTP request path
  if (!m_strProtocol.equals("mailto:",7))    //a_Mailto does not need the path
    result.append(getPathFileAndQueryString());
}

void AUrl::_emitDataProtocol(AOutputBuffer& result) const
{
  AASSERT(this, m_strProtocol.equals("data:",5));
  result.append("data:",5);
  result.append(m_strServer);
  
  m_QueryString.emitDelimeted(result, AConstant::ASTRING_EQUALS, AConstant::ASTRING_SEMICOLON, false);

  if (m_isBase64)
  {
    result.append(ASW(";base64,",8));
    ATextConverter::encodeBase64(m_strFilename, result);
  }
  else
  {
    result.append(',');
    ATextConverter::decodeURL(m_strFilename, result, false, false);
  }
}

void AUrl::_logError(const AString &strError)
{
  m_strError += ASW("{",1) + strError + '}';
}

int AUrl::compare(const AUrl& source) const
{  
  int iRet = 0x0;
  iRet |= m_strProtocol.compare(source.m_strProtocol);
  iRet |= m_strUsername.compare(source.m_strUsername);
  iRet |= m_strPassword.compare(source.m_strPassword);
  iRet |= m_strServer.compare(source.m_strServer);
  iRet |= (m_iPort == source.m_iPort ? 0x0 : 0x1);
  iRet |= m_strPath.compare(source.m_strPath);
  iRet |= m_strFilename.compare(source.m_strFilename);
//a_Query string not part of the compare  iRet |= m_QueryString.compare(source.m_QueryString);
 
  return iRet;
}

void AUrl::toAFile(AFile &aFile) const
{
  AString url;
  emit(url);
  url.toAFile(aFile);
}

void AUrl::fromAFile(AFile &aFile)
{
  AString url;
  url.fromAFile(aFile);
  parse(url);
}

AString AUrl::getQueryString() const
{
  AString str; 
  m_QueryString.emit(str);
  return str; 
}

AString AUrl::getPathAndFilename() const 
{ 
  return m_strPath + m_strFilename;
} 

AString AUrl::getPathAndFilenameNoExt() const 
{ 
  return m_strPath + getFilenameNoExt();
} 

void AUrl::setProtocol(const AString &strProtocol)
{ 
  if (strProtocol[strProtocol.getSize() - 1] != ':')
    ATHROW(this, AException::InvalidProtocol);
  
  m_strProtocol = strProtocol;
  m_strProtocol.makeLower();

  //a_Now try to assign a default port if it's a known protocol
  //a_Only if the port is not valid
  if (m_iPort == AUrl::INVALID)
  {
	  if (m_strProtocol.equals("http:",5)) { m_iPort = AUrl::HTTP; return; }
	  if (m_strProtocol.equals("data:",5)) { m_iPort = AUrl::DATA; return; }
	  if (m_strProtocol.equals("https:",6)) { m_iPort = AUrl::HTTPS; return; }
	  if (m_strProtocol.equals("ftp:",4)) { m_iPort = AUrl::FTP; return; }
	  if (m_strProtocol.equals("file:",5)) { m_iPort = AUrl::FILE; return; }
	  if (m_strProtocol.equals("mysql:",6)) { m_iPort = AUrl::MYSQL; return; }
	  if (m_strProtocol.equals("odbc:",5)) { m_iPort = AUrl::ODBC; return; }
    if (m_strProtocol.equals("sqlite:",7)) { m_iPort = AUrl::SQLITE; return; }
	  if (m_strProtocol.equals("news:",5)) { m_iPort = AUrl::NNTP; return; }
	  if (m_strProtocol.equals("mailto:",7)) { m_iPort = AUrl::SMTP; return; }
	  if (m_strProtocol.equals("gopher:",7)) { m_iPort = AUrl::GOPHER; return; }
	  if (m_strProtocol.equals("telnet:",7)) { m_iPort = AUrl::TELNET; return; }
	  if (m_strProtocol.equals("wais:",5)) { m_iPort = AUrl::WAIS; return; }
	  if (m_strProtocol.equals("prospero:",9)) { m_iPort = AUrl::PROSPERO; return; }
    m_iPort = AUrl::NONE;
  }
}

AUrl::eProtocol AUrl::getProtocolEnum() const
{
	if (m_strProtocol.equals("http:",5)) { return AUrl::HTTP; }
	if (m_strProtocol.equals("data:",5)) { return AUrl::DATA; }
	if (m_strProtocol.equals("https:",6)) { return AUrl::HTTPS; }
	if (m_strProtocol.equals("ftp:",4)) { return AUrl::FTP; }
	if (m_strProtocol.equals("file:",5)) { return AUrl::FILE; }
	if (m_strProtocol.equals("mysql:",6)) { return AUrl::MYSQL; }
	if (m_strProtocol.equals("odbc:",5)) { return AUrl::ODBC; }
	if (m_strProtocol.equals("sqlite:",7)) { return AUrl::SQLITE; }
	if (m_strProtocol.equals("news:",5)) { return AUrl::NNTP; }
	if (m_strProtocol.equals("mailto:",7)) { return AUrl::SMTP; }
	if (m_strProtocol.equals("gopher:",7)) { return AUrl::GOPHER; }
	if (m_strProtocol.equals("telnet:",7)) { return AUrl::TELNET; }
	if (m_strProtocol.equals("wais:",5)) { return AUrl::WAIS; }
	if (m_strProtocol.equals("prospero:",9)) { return AUrl::PROSPERO; }
  return AUrl::INVALID;
}

void AUrl::setUsername(const AString &strUsername)
{ 
  m_strUsername = strUsername;
}

void AUrl::setPassword(const AString &strPassword)
{ 
  m_strPassword = strPassword;
}

void AUrl::setServer(const AString &strServer)
{
  m_strServer = strServer;
}

void AUrl::setPort(int iPort)
{ 
  m_iPort = iPort;
}

void AUrl::setPath(const AString &strPath)
{ 
  m_strPath = strPath;
}

void AUrl::setFilename(const AString &strFilename)
{ 
  m_strFilename = strFilename;
}

void AUrl::setExtension(const AString &strExt)
{ 
  m_strFilename = getFilenameNoExt() + AConstant::ASTRING_PERIOD + strExt;
}

void AUrl::setFilenameNoExt(const AString &strName)
{ 
  m_strFilename = strName + AConstant::ASTRING_PERIOD + getExtension();
}

void AUrl::setPathAndFilename(const AString &strPath)
{
  _parsePath(strPath);
}

const AString& AUrl::getProtocol() const
{
  return m_strProtocol;
}

const AString& AUrl::getUsername() const
{ 
  return m_strUsername;
}

const AString& AUrl::getPassword() const
{ 
  return m_strPassword;
}

const AString& AUrl::getServer() const
{ 
  return m_strServer;
}

int AUrl::getPort() const
{ 
  return m_iPort; 
}

const AString& AUrl::getPath() const
{
  return m_strPath;
}

const AString& AUrl::getFilename() const
{ 
  return m_strFilename;
}

bool AUrl::isProtocol(AUrl::eProtocol e) const
{ 
  return (getProtocolEnum() == e ? true : false);
}

bool AUrl::isExtension(const AString& ext) const
{
  size_t pos = m_strFilename.rfind('.');
  if (AConstant::npos != pos)
  {
    AString str;
    m_strFilename.peek(str, pos + 0x1);
    return str.equals(ext);
  }
  else
    return false;
}

AString AUrl::getFilenameNoExt() const
{
  size_t pos = m_strFilename.rfind('.');
  AString str;
  if (AConstant::npos != pos)
  {
    m_strFilename.peek(str, 0, pos);
    return str;
  }
  else
    return m_strFilename;
}

AString AUrl::getExtension() const
{
  size_t pos = m_strFilename.rfind('.');
  if (AConstant::npos != pos)
  {
    AString str;
    m_strFilename.peek(str, pos + 0x1);
    return str;
  }
  else
    return AConstant::ASTRING_EMPTY;
}

AString AUrl::toAString() const
{
  AString str;
  emit(str);
  return str;
}

AQueryString& AUrl::useParameterPairs()
{ 
  return m_QueryString; 
}

const AQueryString& AUrl::getParameterPairs() const 
{ 
  return m_QueryString;
}

AString& AUrl::useMediaType()
{
  return m_strServer;
}

bool AUrl::isBase64Encoded() const
{
  return m_isBase64;
}

bool AUrl::isValid() const
{
  if (AConstant::npos != m_strPath.findNotOneOf(AConstant::CHARSET_URL_RFC2396))
    return false;

  if (AConstant::npos != m_strFilename.findNotOneOf(AConstant::CHARSET_URL_RFC2396))
    return false;

  return true;
}

void AUrl::setIsBase64Encoded(bool b)
{
  m_isBase64 = b;
}

AString& AUrl::useData()
{
  return m_strFilename;
}

const AString& AUrl::getError() const
{ 
  return m_strError;
}
