/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AObjectContainer_HPP__
#define INCLUDED__AObjectContainer_HPP__

#include "apiABase.hpp"
#include "AObjectBase.hpp"
#include "templateAObject.hpp"
#include "AException.hpp"

/*!
Path examples:
  /path0/path1/objectname - object is placed in /path0/path1/ and named 'objectname'
  /path0/                 - object placed in /path0/ and it must have been named during creation

NOTE: AObjectContainer OWNS the objects it contains and will delete them when finished

@deprecated Use ABasePtrContainer instead
*/
class ABASE_API AObjectContainer : public AObjectBase
{
public:
  typedef std::map<AString, AObjectBase *> MAP_STRING_OBJECTBASE;
  typedef std::pair<AString, AObjectBase *> MAP_PAIR;

public:
  AObjectContainer(const AString& name = AConstant::ASTRING_EMPTY);
  AObjectContainer(const AObjectContainer&);
  virtual ~AObjectContainer();

  /*!
  AEmittable and AXmlEmittable
  */
  virtual void emit(AOutputBuffer& target) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Emit XML based on a subpath
  */
  virtual void emit(AOutputBuffer&, const AString& path) const;
  virtual void emitXml(AXmlElement&, const AString& path) const;

  /*!
  Insertion of objects
  Path is /path0/path1/objectname  - object name is REQUIRED if object inserted is nameless, objectname will override the name of the object inserted
  Value can be AObjectBase based object or AString (which will get wrapped in AObject<AString> object)
  insert() returns a reference to the AObjectBase that was just inserted
  */
  AObjectBase& insert(const AString& path, const AString& value = AConstant::ASTRING_EMPTY, AXmlElement::Encoding encoding = AXmlElement::ENC_NONE);
  
  /*!
  Insert AObjectBase type into a given path
  If pobject is NULL exception is thrown
  Object is OWNED by this container and will be deleted when done
  */
  AObjectBase& insert(const AString& path, AObjectBase *);
  
  /*!
  Remove object at path
  */
  void remove(const AString& path);
  
  /*!
  Clear container
  */
  void clear();

  /*!
  If ID exists, then this object has already been saved and unique id assigned during save()
  */
  bool getID(AString& target) const;

  /*!
  Get AString type (emit() will be called on complex objects
  */
  bool get(const AString& path, AString& target) const;

  /*!
  Basic get, NULL if not found
  */
  AObjectBase *getObject(const AString& path) const;
  bool exists(const AString& path) const;                    //a_Checks existance (wrapper to getObject)

  /*!
  Usage: AString *p = ns.getAs<AString>("/somepath/someobject");
    if someobject exists and is AString you get a valid pointer to AString or NULL
  */
  template<typename _type> _type *getAsPtr(const AString& path) const
  {
    AObjectBase *ptr = getObject(path);
    if (ptr)
    {
      //a_First try AObjectBase types in cast
      {
        _type *pup = dynamic_cast<_type *>(ptr);
        if (pup)
          return pup;
      }

      //a_Now objects wrapped by AObject template
      {
        AObject<_type> *pup = dynamic_cast< AObject<_type> *>(ptr);
        if (pup)
          return &(pup->use());
      }
    }
    
    return NULL;  //a_Either type mismatched or it was not found
  }

  /*!
  AObjectException::TypeidMismatch if wrong type requested
  AException::DoesNotExist if no such item
  */
  template<typename _type> _type useAs(const AString& path) const
  {
    AObjectBase *ptr = get(path);
    if (ptr)
    {
      AObject<_type> *pup = dynamic_cast< AObject<_type> *>(ptr);
      if (pup) return pup->use();
      else ATHROW_EX(this, AException::TypeidMismatch, path);   //a_Type mismatch
    }
    else ATHROW_EX(this, AException::DoesNotExist, path);       //a_Does not exist
  }

  //a_AObjectBase clone
  virtual AObjectBase* clone() const { return new AObjectContainer(*this); }
  
  /*!
  Access to the container
  */
  const MAP_STRING_OBJECTBASE& getContainer() const { return m_Objects; }

  /*!
  String constants
  */
  static const AString ID;      // Object id tag

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  MAP_STRING_OBJECTBASE m_Objects;

private:
  //a_Internal access
  AObjectBase &_insertObjectPtr(const AString& path, AObjectBase *pobject);
  
  //a_If NULL container could not be created due to error or boolCreate flag was false
  AObjectContainer *_getContainer(LIST_AString&, bool boolCreate);
  AObjectContainer *_getContainer(LIST_AString&) const;
};

#endif //INCLUDED__AObjectContainer_HPP__
