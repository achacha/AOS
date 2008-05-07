#include "pchABase.hpp"
#include "ACookieJar.hpp"

void ACookieJar::Node::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<" @ " << std::hex << this << std::dec << ") { " << std::endl;
  
  for(ACookieJar::Node::COOKIES::TYPEDEF::const_iterator citCookie = m_Cookies._map.begin(); citCookie != m_Cookies._map.end(); ++citCookie)
  {
    citCookie->second->debugDump(os, indent+1);
  }

  for (ACookieJar::Node::NODES::TYPEDEF::const_iterator citNode = m_Nodes._map.begin(); citNode != m_Nodes._map.end(); ++citNode)
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
    for (ACookieJar::Node::COOKIES::TYPEDEF::const_iterator cit = p->m_Cookies._map.begin(); cit != p->m_Cookies._map.end(); ++cit)
      cit->second->emitRequestHeaderString(target);
  
    p = p->mp_Parent;
  }
}

void ACookieJar::Node::emitSecureOnly(AOutputBuffer& target) const 
{
  const Node *p = this;
  while (p)
  {
    for (ACookieJar::Node::COOKIES::TYPEDEF::const_iterator cit = p->m_Cookies._map.begin(); cit != p->m_Cookies._map.end(); ++cit)
    {
      if (cit->second->isSecure())
        cit->second->emitRequestHeaderString(target);
    }

    p = p->mp_Parent;
  }
}
