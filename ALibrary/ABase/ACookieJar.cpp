/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "ACookieJar.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"

void ACookieJar::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<" @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Domains={" << std::endl;
  for(DOMAINS::const_iterator cit = m_Domains.begin(); cit != m_Domains.end(); ++cit)
  {
    ADebugDumpable::indent(os, indent+2) << cit->first << "={" << std::endl;
    cit->second->debugDump(os, indent+3);
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ACookieJar::ACookieJar()
{
}

ACookieJar::~ACookieJar()
{
}

void ACookieJar::add(ACookie *pcookie)
{ 
  if (pcookie->getDomain().isEmpty())
    ATHROW_EX(pcookie, AException::InvalidObject, ASWNL("Cookie did not specify a valid domain"));

  Node *p = _getNode(pcookie->getDomain(), pcookie->getPath(), true);
  p->m_Cookies[pcookie->getName()] = pcookie;
}

ACookieJar::Node *ACookieJar::_getNode(const AString& domain, const AString& path, bool createIfNotFound)
{
  Node *pCurrentNode = NULL;
  DOMAINS::iterator itDomain = m_Domains.find(domain);
  if (itDomain == m_Domains.end())
  {
    //a_Domain does not exist
    if (!createIfNotFound)
      return NULL;

    //a_Insert new root node
    pCurrentNode = new Node();
    m_Domains[domain] = pCurrentNode;
  }
  else
  {
    pCurrentNode = itDomain->second;
  }
  AASSERT(this, pCurrentNode);

  LIST_AString dirs;
  path.split(dirs, '/');
  while (dirs.size() > 0)
  {
    AString& dir = dirs.front();
    ACookieJar::Node::NODES::iterator it = pCurrentNode->m_Nodes.find(dir);
    if (it != pCurrentNode->m_Nodes.end())
    {
      //a_Dir already exists
      pCurrentNode = it->second;
    }
    else
    {
      if (createIfNotFound)
      {
        //a_Dir not found, create
        Node *pNewNode = new Node(pCurrentNode);
        pCurrentNode->m_Nodes[dir] = pNewNode;
        pCurrentNode = pNewNode;
      }
      else
        return NULL;  //a_Not found and not creating
    }
    dirs.pop_front();
  }

  return pCurrentNode;
}

const ACookieJar::Node *ACookieJar::_getNodeOrExistingParent(const AString& domain, const AString& path) const
{
  DOMAINS::const_iterator citDomain = m_Domains.find(domain);
  if (citDomain == m_Domains.end())
    return NULL;  // Domain does not exist

  const Node *pCurrentNode = citDomain->second;

  LIST_AString dirs;
  path.split(dirs, '/');
  while (dirs.size() > 0)
  {
    AString& dir = dirs.front();
    ACookieJar::Node::NODES::const_iterator cit = pCurrentNode->m_Nodes.find(dir);
    if (cit == pCurrentNode->m_Nodes.end())
      break;  //a_Return current node, sub node not found
    else
      pCurrentNode = cit->second;

    dirs.pop_front();
  }
  return pCurrentNode;
}

void ACookieJar::emit(AOutputBuffer& target) const 
{ 
  for (DOMAINS::const_iterator cit = m_Domains.begin(); cit != m_Domains.end(); ++cit)
  {
    target.append(cit->first);
    target.append(AConstant::ASTRING_CRLF);
  }
}

void ACookieJar::emit(
  AOutputBuffer& target, 
  const AString& domain, 
  const AString& path, 
  bool secureOnly         // = false
) const
{
  const Node *p = _getNodeOrExistingParent(domain, path);
  if(p)
  {
    if (secureOnly)
      p->emitSecureOnly(target);
    else
      p->emit(target);
  }
}

void ACookieJar::parse(const AHTTPResponseHeader& header)
{
  const ACookies& cookies = header.getCookies();
  for (ACookies::CONTAINER::const_iterator cit = cookies.getContainer().begin(); cit != cookies.getContainer().end(); ++cit)
  {
    if ((*cit)->isExpired())
      remove((*cit)->getDomain(), (*cit)->getPath(), (*cit)->getName());
    else
      add(new ACookie(*(*cit)));
  }
}

bool ACookieJar::remove(const AString& domain, const AString& path, const AString& name)
{
  Node *pNode = _getNode(domain, path);
  if (pNode)
  {
    ACookieJar::Node::COOKIES::iterator it = pNode->m_Cookies.find(name);
    if (it != pNode->m_Cookies.end())
    {
      delete it->second;
      pNode->m_Cookies.erase(it);
      return true;
    }
    else
      return false;  //a_No such cookie name
  }
  else
    return false; //a_DNE
}

void ACookieJar::emit(AHTTPRequestHeader& header)
{
  AString str;
  emit(str, header.getUrl().getServer(), header.getUrl().getPath(), header.getUrl().isProtocol(AUrl::HTTPS));
  if (!str.isEmpty())
    header.setPair(AHTTPRequestHeader::HT_REQ_Cookie, str);
}
