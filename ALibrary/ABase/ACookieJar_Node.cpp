#include "pchABase.hpp"
#include "ACookieJar.hpp"

void ACookieJar::Node::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<" @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Parent=" << AString::fromPointer(mp_Parent) << std::endl;
  
  for(ACookieJar::Node::COOKIES::TYPEDEF::const_iterator citCookie = m_Cookies._list.begin(); citCookie != m_Cookies._list.end(); ++citCookie)
  {
    ADebugDumpable::indent(os, indent+1) << *(*citCookie) << std::endl;
  }

  for (ACookieJar::Node::NODES::TYPEDEF::const_iterator citNode = m_Nodes._map.begin(); citNode != m_Nodes._map.end(); ++citNode)
  {
    ADebugDumpable::indent(os, indent+1) << citNode->first << "={" << std::endl;
    citNode->second->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
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
    for (ACookieJar::Node::COOKIES::TYPEDEF::const_iterator cit = p->m_Cookies._list.begin(); cit != p->m_Cookies._list.end(); ++cit)
      (*cit)->emitRequestHeaderString(target);
  
    p = p->mp_Parent;
  }
}

void ACookieJar::Node::emitSecureOnly(AOutputBuffer& target) const 
{
  const Node *p = this;
  while (p)
  {
    for (ACookieJar::Node::COOKIES::TYPEDEF::const_iterator cit = p->m_Cookies._list.begin(); cit != p->m_Cookies._list.end(); ++cit)
    {
      if ((*cit)->isSecure())
        (*cit)->emitRequestHeaderString(target);
    }

    p = p->mp_Parent;
  }
}
