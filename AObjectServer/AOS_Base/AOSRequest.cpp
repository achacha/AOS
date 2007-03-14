#include "pchAOS_Base.hpp"
#include "AOSRequest.hpp"
#include "AException.hpp"
#include "AFile_Socket.hpp"
#include "AXmlElement.hpp"
#include "AOSServices.hpp"

#ifdef __DEBUG_DUMP__
void AOSRequest::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSRequest @ " << std::hex << this << std::dec << ") {" << std::endl;

  //a_Input/Output file
  if (mp_RequestFile)
  {
    ADebugDumpable::indent(os, indent+1) << "*mp_RequestFile=" << std::endl;
    mp_RequestFile->debugDump(os, indent+2);
  }
  else
    ADebugDumpable::indent(os, indent+1) << "mp_Request=NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_RequestTimer=" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_isHttps=" << (m_isHttps ? AString::sstr_True : AString::sstr_False) << std::endl;
  m_RequestTimer.debugDump(os, indent+2);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSRequest::AOSRequest(AOSServices& services, AFile_Socket *pFile) :
  m_Services(services),
  mp_RequestFile(pFile),
  m_RequestTimer(true),
  m_isHttps(false)
{
  AASSERT(this, pFile);
}

AOSRequest::~AOSRequest()
{
  pDelete(mp_RequestFile);
}

AFile_Socket& AOSRequest::useSocket()
{
#ifdef __DEBUG_DUMP__
  if (!ADebugDumpable::isPointerValid(mp_RequestFile))
    ATHROW_EX(this, AException::InvalidObject, AString("mp_RequestFile")+AString::fromPointer(mp_RequestFile));
#endif

  if (mp_RequestFile)
    return *mp_RequestFile;
  else
    ATHROW(this, AException::InvalidObject);
}

void AOSRequest::emit(AXmlElement& target) const
{
  AASSERT(this, !target.useName().isEmpty());
  target.addElement(ASW("https",5), m_isHttps); 
  target.addElement(ASW("timer",5), m_RequestTimer); 
}

const ATimer& AOSRequest::getTimer() const
{
  return m_RequestTimer;
}

bool AOSRequest::isHttps() const
{
  return m_isHttps;
}

void AOSRequest::setHttps(bool is)
{
  m_isHttps = is;
}

AOSRequest *AOSRequest::allocate(AOSServices& services, AFile_Socket *pfile)
{
  AOSRequest *pRequest = new AOSRequest(services, pfile);

  static const AString LOG_MSG("AOSRequest::allocate");
  services.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);

  return pRequest;
}

void AOSRequest::deallocate(AOSRequest *p)
{
  static const AString LOG_MSG("AOSRequest::deallocate");
  p->m_Services.useLog().add(LOG_MSG, AString::fromPointer(p), ALog::DEBUG);

  delete p;
}
