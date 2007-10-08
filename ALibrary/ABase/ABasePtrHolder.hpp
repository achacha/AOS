#ifndef INCLUDED__ABasePtrHolder_HPP__
#define INCLUDED__ABasePtrHolder_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AException.hpp"
#include "templateAObject.hpp"

/*!
A way to associate a name to an object pointer

NOTE: ABasePtrHolder only holds pointers to ABase types, it does NOT delete and may point to stale data, caveat emptor
*/
class ABASE_API ABasePtrHolder : public ADebugDumpable
{
public:
  typedef std::map<AString, ABase *> MAP_ASTRING_ABASE;

public:
  ABasePtrHolder();
  virtual ~ABasePtrHolder();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Insertion of objects
  Associates AObjectBase pointer with name
  
  NOTE: Will NOT delete this object
  */
  void insert(const AString& name, ABase *, bool overwrite = true);
  
  /*!
  Insertion of objects
  Associates AObjectBase pointer with name

  NOTE: Will DELETE this object
  */
  void insertWithOwnership(const AString& name, ABase *, bool overwrite = true);

  /*!
  Remove object at path
  */
  void remove(const AString& name);
  
  /*!
  Clear container
  */
  void clear();

  /*!
  Get pointer stored, NULL if not found
  */
  ABase *get(const AString& name) const;

  /*!
  Usage: AXmlDocument *pDoc = ns.getAs<AXmlDocument>("xmldoc");
    if xmldoc exists, it will be dynamically cast to AXmlDocument (in this example) and if types match it will be non-NULL
    NULL returns means either it doesn't exist or it is not of a specified pointer type
  */
  template<typename _type> _type *getAsPtr(const AString& name) const
  {
    ABase *ptr = get(name);
    if (ptr)
    {
      _type *pup = dynamic_cast<_type *>(ptr);
      if (pup)
        return pup;
    }
    
    return NULL;  //a_Either type mismatched or it was not found
  }

  /*!
  Access to the actual container (for convenience)
  */
  MAP_ASTRING_ABASE& useContainer() { return m_BasePtrs; }

protected:
  MAP_ASTRING_ABASE m_BasePtrs;

  //a_List of objects to delete when done
  typedef std::map<ABase *, bool> MAP_ABASE_PTRS;
  MAP_ABASE_PTRS m_OwnedPtrs;

private:
  ABasePtrHolder(const ABasePtrHolder&) {} // no copy allowed

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ABasePtrHolder_HPP__
