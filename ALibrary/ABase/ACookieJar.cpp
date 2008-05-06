#include "pchABase.hpp"
#include "ACookieJar.hpp"
#include "ATextConverter.hpp"

void ACookieJar::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<" @ " << std::hex << this << std::dec << ") { " << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Root={" << std::endl;
  mp_Root->debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ACookieJar::ACookieJar() :
  mp_Root(new Node())
{
}

ACookieJar::~ACookieJar()
{
  try
  {
    delete mp_Root;
  }
  catch(...) {}
}

void ACookieJar::add(ACookie *pcookie)
{ 
  Node *p = _getNode(pcookie->getPath(), true);
  p->m_Cookies._list.push_back(pcookie);
}

ACookieJar::Node *ACookieJar::_getNode(const AString& path, bool createIfNotFound)
{
  LIST_AString dirs;
  path.split(dirs, '/');

  Node *pCurrentNode = mp_Root;
  while (dirs.size() > 0)
  {
    AString& dir = dirs.front();
    ACookieJar::Node::NODES::TYPEDEF::iterator it = pCurrentNode->m_Nodes._map.find(dir);
    if (it != pCurrentNode->m_Nodes._map.end())
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
        pCurrentNode->m_Nodes._map[dir] = pNewNode;
        pCurrentNode = pNewNode;
      }
      else
        return NULL;  //a_Not found and not creating
    }
    dirs.pop_front();
  }

  return pCurrentNode;
}

const ACookieJar::Node *ACookieJar::_getNodeOrExistingParent(const AString& path) const
{
  LIST_AString dirs;
  path.split(dirs, '/');

  Node *pCurrentNode = mp_Root;
  while (dirs.size() > 0)
  {
    AString& dir = dirs.front();
    ACookieJar::Node::NODES::TYPEDEF::iterator it = pCurrentNode->m_Nodes._map.find(dir);
    if (it == pCurrentNode->m_Nodes._map.end())
      break;  //a_Return current node, sub node not found
    else
      pCurrentNode = it->second;

    dirs.pop_front();
  }
  return pCurrentNode;
}

void ACookieJar::emit(AOutputBuffer& target) const 
{ 
  emit(target, AConstant::ASTRING_EMPTY);
}

void ACookieJar::emit(
  AOutputBuffer& target, 
  const AString& path, 
  bool secureOnly         // = false
) const
{
  const Node *p = _getNodeOrExistingParent(path);
  if(p)
  {
    if (secureOnly)
      p->emitSecureOnly(target);
    else
      p->emit(target);
  }
}
