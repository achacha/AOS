#ifndef INCLUDED__AObjectPtrHolder_HPP__
#define INCLUDED__AObjectPtrHolder_HPP__

#include "apiABase.hpp"
#include "AObjectBase.hpp"
#include "AException.hpp"
#include "templateAObject.hpp"

/*!
A way to associate a name to an object pointer

NOTE: AObjectPtrHolder only holds pointers to objects, it does NOT delete and may point to stal data, caveat emptor
*/
class ABASE_API AObjectPtrHolder : public AObjectBase
{
public:
  typedef std::map<AString, AObjectBase *> MAP_ASTRING_AOBJECTBASE;
  typedef std::pair<AString, AObjectBase *> MAP_PAIR;

public:
  AObjectPtrHolder(const AString& name = AConstant::ASTRING_EMPTY);
  AObjectPtrHolder(const AObjectPtrHolder&);
  virtual ~AObjectPtrHolder();

  /*!
  AEmittable and AXmlEmittable
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

  /*!
  Insertion of objects
  Associates AObjectBase pointer with name
  */
  void insert(const AString& name, AObjectBase *, bool overwrite = true);
  
  /*!
  Remove object at path
  */
  void remove(const AString& name);
  
  /*!
  Clear container
  */
  void clear();

  /*!
  Get AString type (emit() will be called on complex objects
  */
  AObjectBase *get(const AString& name) const;

  /*!
  Usage: AXmlDocument *pDoc = ns.getAs<AXmlDocument>("xmldoc");
    if xmldoc exists, it will be dynamically cast to AXmlDocument (in this example) and if types match it will be non-NULL
    NULL returns means either it doesn't exist or it is not of a specified pointer type
  */
  template<typename _type> _type *getAsPtr(const AString& name) const
  {
    AObjectBase *ptr = get(name);
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

  //a_AObjectBase clone
  virtual AObjectBase* clone() const { return new AObjectPtrHolder(*this); }
  
  /*!
  Access to the actual container
  */
  const MAP_ASTRING_AOBJECTBASE& getContainer() const { return m_ObjectPointers; }

protected:
  MAP_ASTRING_AOBJECTBASE m_ObjectPointers;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AObjectPtrHolder_HPP__
