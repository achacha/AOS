#include "pchABase.hpp"

#include "AObjectContainer.hpp"
#include "AException.hpp"
#include "ARope.hpp"
#include "ATextGenerator.hpp"
#include "AUrl.hpp"

const AString AObjectContainer::ID("id");

void AObjectContainer::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AObjectContainer[\"" << m_Name << "\"] @ " << std::hex << this << std::dec << ") {" << std::endl;

  MAP_STRING_OBJECTBASE::const_iterator cit = m_Objects.begin();
  while (cit != m_Objects.end())
  {
    (*cit).second->debugDump(os, indent+1);
    ++cit;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AObjectContainer::AObjectContainer(const AString& name /* = AConstant::ASTRING_EMPTY */) :
  AObjectBase(name)
{
}

AObjectContainer::AObjectContainer(const AObjectContainer& that) :
  AObjectBase(that)
{
  MAP_STRING_OBJECTBASE::const_iterator cit = that.m_Objects.begin();
  while (cit != that.m_Objects.end())
  {
    m_Objects.insert(MAP_PAIR((*cit).first, (*cit).second->clone()));
    ++cit;
  }
}

AObjectContainer::~AObjectContainer()
{
  try
  {
    MAP_STRING_OBJECTBASE::iterator it = m_Objects.begin();
    while (it != m_Objects.end())
    {
      delete (*it).second;
      ++it;
    }
  }
  catch(...)
  {
    //a_Prevent propagation of exception if this during exception cleanup
  }
}

AObjectBase &AObjectContainer::insert(const AString& path, AObjectBase *pobject)
{
  if (!pobject)
    ATHROW(this, AException::InvalidParameter);

  return _insertObjectPtr(path, pobject);
}

AObjectBase &AObjectContainer::insert(const AString& path, const AString& value, AXmlElement::Encoding encoding)
{
  AObjectBase *pobject = new AObject<AString>(value, encoding);
  
  return _insertObjectPtr(path, pobject);
}

bool AObjectContainer::get(const AString& path, AString& target) const
{
  AObjectBase *p = getObject(path);
  target.clear();
  if (p)
  {
    p->emit(target);
    return true;
  }
  else
  {
    return false;
  }
}

bool AObjectContainer::exists(const AString& path) const
{
  return (NULL != getObject(path) ? true : false);
}

AObjectBase *AObjectContainer::getObject(const AString& path) const
{
  AUrl url(path);
  LIST_AString names;
  url.getPath().split(names, '/');
  
  const AObjectContainer *pObjects = (names.size() == 0 ? this : _getContainer(names));
  if (pObjects && AConstant::ASTRING_EMPTY != url.getFilename())
  {
    MAP_STRING_OBJECTBASE::const_iterator cit = pObjects->m_Objects.find(url.getFilename());
    if (cit != pObjects->m_Objects.end())
      return (*cit).second;
  }

  return NULL;
}

void AObjectContainer::remove(const AString& path)
{
  MAP_STRING_OBJECTBASE::iterator it = m_Objects.find(path);
  if (it != m_Objects.end())
  {
    delete (*it).second;
    m_Objects.erase(it);
  }
}

AObjectBase &AObjectContainer::_insertObjectPtr(const AString& path, AObjectBase *pobject)
{
  if (!pobject)
    ATHROW(this, AException::InvalidParameter);

  AUrl url(path);
  LIST_AString names;
  url.getPath().split(names, '/');
  AObjectContainer *pObjects = _getContainer(names, true);
  
  //a_Name provided in path, rename object (path always overrides the object name)
  if (AConstant::ASTRING_EMPTY != url.getFilename())
    pobject->setName(url.getFilename());

  //a_Container path was created, but without an object nothing further needs to be done
  if (pobject->getName().isEmpty())
    ATHROW(this, AException::CannotInsertUnnamedObject);

  if (pObjects)
  {
    MAP_STRING_OBJECTBASE::iterator it = pObjects->m_Objects.find(url.getFilename());
    if (it != pObjects->m_Objects.end())
    {
      //a_Replace
      delete (*it).second;
      (*it).second = pobject;
    }
    else
    {
      //a_Create
      pObjects->m_Objects.insert(MAP_PAIR(url.getFilename(), pobject));
    }
  }
  else
    ATHROW(this, AException::OperationFailed);  //a_Unable to create a container

  return *pobject;
}

AObjectContainer *AObjectContainer::_getContainer(LIST_AString& names, bool boolCreate)
{
  if (names.size() == 0)
    return this;

  //a_Look in immediate container
  AString nsname = names.front();
  names.pop_front();
  MAP_STRING_OBJECTBASE::iterator it = m_Objects.find(nsname);
  if (it == m_Objects.end() && boolCreate)
  {
    //a_Create new container and insert it (if one does not exist, the sub containers don't either)
    AObjectContainer *pObjects = new AObjectContainer(nsname);
    m_Objects.insert(MAP_PAIR(nsname, pObjects));

    while (names.size() > 0) 
    {
      nsname = names.front();
      names.pop_front();
      AObjectContainer *pObjectsSub = new AObjectContainer(nsname);
      pObjects->m_Objects.insert(MAP_PAIR(nsname, pObjectsSub));
      pObjects = pObjectsSub;
    }
    return pObjects;
  }
  else
  {
    //a_Container exists
    AObjectBase *p = (*it).second;
    AObjectContainer *pObjects = dynamic_cast<AObjectContainer *>(p);
    if (p == pObjects)
    {
      AASSERT(this, pObjects);
      if (pObjects && names.size() > 0)
        return pObjects->_getContainer(names, boolCreate);
      else
        return pObjects;
    }
    else
      ATHROW(this, AException::ObjectContainerCollision);
  }
}

AObjectContainer *AObjectContainer::_getContainer(LIST_AString& names) const
{
  if (names.size() == 0)
    ATHROW(this, AException::ProgrammingError);  //a_Should never be here if there is no path

  //a_Look in immediate container
  AString nsname = names.front();
  names.pop_front();
  MAP_STRING_OBJECTBASE::const_iterator cit = m_Objects.find(nsname);
  if (cit != m_Objects.end())
  {
    AObjectBase *p = (*cit).second;
    AObjectContainer *pObjects = dynamic_cast<AObjectContainer *>(p);
    if (p == pObjects)
    {
      if (names.size() > 0)
        return pObjects->_getContainer(names);
      else
        return pObjects;
    }
    else
      ATHROW(this, AException::ObjectContainerCollision);
  }

  return NULL;
}

void AObjectContainer::clear()
{
  MAP_STRING_OBJECTBASE::iterator it = m_Objects.begin();
  while (it != m_Objects.end())
  {
    delete (*it).second;
    ++it;
  }
  
  m_Objects.clear();
}

bool AObjectContainer::getID(AString& target) const
{
  return (!get(ID, target) ? false : true);
}

void AObjectContainer::emit(AOutputBuffer& target) const
{
  emit(target, AConstant::ASTRING_EMPTY);
}

AXmlElement& AObjectContainer::emitXml(AXmlElement& thisRoot) const
{
  emitXml(thisRoot, AConstant::ASTRING_EMPTY);
  return thisRoot;
}

void AObjectContainer::emit(AOutputBuffer& target, const AString& strPath) const
{
  if (strPath.isEmpty())
  {
    MAP_STRING_OBJECTBASE::const_iterator cit = m_Objects.begin();
    //a_Recurse objects
    while (cit != m_Objects.end())
    {
      (*cit).second->emit(target);
      ++cit;
      if (cit != m_Objects.end())
        target.append(AConstant::ASTRING_COMMA);
    }
  }
  else
  {
    AObjectBase *pObj = getObject(strPath);
    if (pObj)
    {
      pObj->emit(target);
    }
    else
      ATHROW_EX(this, AException::DoesNotExist, AString("path specified does not exist:")+strPath);
  }
}

void AObjectContainer::emitXml(AXmlElement& thisRoot, const AString& strPath) const
{
  if (strPath.isEmpty())
  {
    if (m_Name.isEmpty())
      ATHROW_EX(this, AException::ProgrammingError, ASWNL("Either AObjectContainer or AXmlElement must have a name"));

    if (thisRoot.useName().isEmpty())
      thisRoot.useName().assign(m_Name);
    
    thisRoot.useAttributes() = m_Attributes;

    MAP_STRING_OBJECTBASE::const_iterator cit = m_Objects.begin();
    //a_Recurse objects
    while (cit != m_Objects.end())
    {
      AAutoPtr<AXmlElement> pself(new AXmlElement((*cit).first));     //a_Create a child
      (*cit).second->emitXml(*pself);
      thisRoot.addContent(pself);
      pself.setOwnership(false);
      ++cit;
    }
  }
  else
  {
    AObjectBase *pObj = getObject(strPath);
    if (pObj)
    {
      pObj->emitXml(thisRoot);
    }
    else
      ATHROW_EX(this, AException::DoesNotExist, AString("path specified does not exist:")+strPath);
  }
}

