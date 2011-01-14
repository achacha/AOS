/*
Written by Alex Chachanashvili

$Id: ACookieJar_Node.cpp 291 2009-07-04 23:16:41Z achacha $
*/
#include "pchAPlatform.hpp"
#include "ACookieJar.hpp"

void ACookieJar::Node::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<" @ " << std::hex << this << std::dec << ") { " << std::endl;
  
  for(ACookieJar::Node::COOKIES::TYPE::const_iterator citCookie = m_Cookies.get().begin(); citCookie != m_Cookies.get().end(); ++citCookie)
  {
    citCookie->second->debugDump(os, indent+1);
  }

  for (ACookieJar::Node::NODES::TYPE::const_iterator citNode = m_Nodes.get().begin(); citNode != m_Nodes.get().end(); ++citNode)
  {
    ADebugDumpable::indent(os, indent+1) << citNode->first << "={" << std::endl;
    citNode->second->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

  ADebugDumpable::indent(os, indent+1) << "mp_Parent=" << AString::fromPointer(mp_Parent) << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ACookieJar::Node::Node(Node *parent) :
  mp_Parent(parent)
{
}

void ACookieJar::Node::emit(AOutputBuffer& target) const 
{
  const Node *p = this;
  while (p)
  {
    for (ACookieJar::Node::COOKIES::TYPE::const_iterator cit = p->m_Cookies.get().begin(); cit != p->m_Cookies.get().end(); ++cit)
      cit->second->emitRequestHeaderString(target);
  
    p = p->mp_Parent;
  }
}

void ACookieJar::Node::emitCookies(AOutputBuffer& target, bool secureOnly)
{
  Node *pNode = this;
  while (pNode)
  {
    ACookieJar::Node::COOKIES::TYPE::iterator it = pNode->m_Cookies.use().begin();
    while(it != pNode->m_Cookies.use().end())
    {
      if (it->second->isExpired())
      {
        ACookieJar::Node::COOKIES::TYPE::iterator itKill = it;
        ++it;

        ACookie *p = itKill->second;
        pNode->m_Cookies.use().erase(itKill);
        delete p;

        continue;
      }
      else
      {
        if ((!secureOnly) || (secureOnly && it->second->isSecure()))
          it->second->emitRequestHeaderString(target);

        ++it;
      }
    }
  
    pNode = pNode->mp_Parent;
  }
}
