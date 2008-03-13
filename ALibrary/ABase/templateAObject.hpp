#ifndef __templateAObject_HPP__
#define __templateAObject_HPP__

#include "apiABase.hpp"
#include "AObjectBase.hpp"
#include "AXmlElement.hpp"
#include "AException.hpp"
#include "ARope.hpp"

/*!
Thin object wrapper for any arbitrary object that can represent itself as a string
This is meant to encapsulate single element objects without having to write a
 derived class for each one, you object just needs to have toString() implemented

What you MUST have to use this wrapper:
  Derive from AEmittable abstract object
     and
  virtual void emit(AOutputBuffer&) const { ... } implemented for your object

e.g.
 container.insert("somepath/someurl1", new AObject<AUrl>( AUrl("http://foo/") );

 container.insert("somepath/someurl2", new AObject<ANumber>() );


@deprecated Use ABase instead
*/
template<class _t>
class AObject : public AObjectBase
{
public:
  /*!
  object - object to copy that will be wrapped
  encoding - data encoding if any to use on the object
  name - object's name (if using AObjectContainer, you can specify object name from the path)
  */
  AObject(AXmlElement::Encoding encoding = AXmlElement::ENC_NONE)
    : AObjectBase(AConstant::ASTRING_EMPTY), m_Encoding(encoding) {}

  AObject(const _t& object, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE)
    : AObjectBase(AConstant::ASTRING_EMPTY), m_Object(object), m_Encoding(encoding) {}
  
  AObject(const _t& object, const AString& name, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE)
    : AObjectBase(name), m_Object(object), m_Encoding(encoding) {}

  AObject(const AObject& that)
    : AObjectBase(that), m_Object(that.m_Object), m_Encoding(that.m_Encoding) {}

  virtual ~AObject() {}

  /*!
  Access to the wrapped object
  */
  _t& use() { return m_Object; }
  operator _t() { return m_Object; }

  /*!
  Sets the output of data (see AXmlData)
  */
  void setDataEncoding(AXmlElement::Encoding encoding) { m_Encoding = encoding; }

  /*!
  Methods needed for AObjectContainer usage and XML publishing
  */
  virtual AObjectBase* clone() const { return new AObject<_t>(*this); }

  /*!
  XML publish, simple object does not handle path
  If an object is wrapped with this template AEmittable MUST be implemented
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const
  {
    if (m_Name.isEmpty())
      ATHROW_EX(this, AException::ProgrammingError, ASWNL("AObjectBase objects must have a name to be emit'd to AXmlElement"));

    thisRoot.useName().assign(m_Name);
    thisRoot.useAttributes() = m_Attributes;

    ARope rope;
    const AEmittable *p = dynamic_cast<const AEmittable *>(&m_Object);
    
    if (p)
    {
      p->emit(rope);
      if (rope.getSize() > 0)
        thisRoot.addData(rope, m_Encoding);
    }
    else
    {
      ATHROW_EX(this, AException::InvalidObject, ARope("Object named '")+m_Name+"' is not derived from AEmittable");
    }

    return thisRoot;
  }

  /*!
  Display as a string, intended for simple objects that don't handle strPath, just uses AEmittable interface
  */
  virtual void emit(AOutputBuffer& target) const
  {
    m_Object.emit(target);
  }

public:
  //a_debugDump() needs to be implemented in the "contained" class to use this method
  void debugDump(std::ostream& os = std::cerr, int indent = 0) const
  {
    ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() <<"[\"" << m_Name << "\"] @ " << std::hex << this << std::dec << ") {" << std::endl;
    m_Object.debugDump(os, indent+1);
    ADebugDumpable::indent(os, indent) << "}" << std::endl; 
  }

private:
  _t m_Object;
  AXmlElement::Encoding m_Encoding;
};

/*!
 Used to upcast from AObjectBase * to AOBject< ... > *
 if upcast fails it returns a NULL

 Usage:
   AObject< AString > objectName(AString("somename"));
   AObjectBase *pbase = dynamic_cast<AObjectBase *>&objectNum;  //a_This is usually a result of some polymorphic storage (e.g. AObjectContainer, etc)
   ...
   AString *pname = asAObject<AString>(pbase);
   if (pname)
     std::cout << *pname << std::endl;
*/
template<typename _type>
_type *asAObject(AObjectBase *ptr)
{
  if (ptr)
  {
    return dynamic_cast< AObject<_type> *>(ptr);
  }
  
  return NULL;
}

/*!
 Used to upcast from AObjectBase * to AOBject< ... > and call the use() method
 if upcast fails, exception is thrown

 Usage:
   AObject< ANumber > objectNum(ANumber("3.14"));
   AObjectBase *pbase = dynamic_cast<AObjectBase *>&objectNum;  //a_This is usually a result of some polymorphic storage (e.g. AObjectContainer, etc)
   ...
   ANumber& mynum = useAObject<ANumber>(pbase);
*/
template<typename _type>
_type useAObject(AObjectBase *ptr)
{
  if (ptr)
  {
    AObject<_type> *pup = dynamic_cast< AObject<_type> *>(ptr);
    if (pup)
      return pup->use();
  }
  
  ATHROW(this, AException::TypeidMismatch);
}

#endif //__templateAObject_HPP__


