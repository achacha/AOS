#ifndef INCLUDED__ABasePtrHolder_HPP__
#define INCLUDED__ABasePtrHolder_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AException.hpp"
#include "templateAutoPtr.hpp"

/*!
A way to associate a name to an ABase auto pointer

NOTE: ABasePtrHolder only holds pointers to ABase types, it does NOT delete and may point to stale data, caveat emptor
*/
class ABASE_API ABasePtrHolder : public ADebugDumpable
{
public:
  //! Holder of AString -> ABase *
  typedef std::map< AString, AAutoPtr<ABase> > HOLDER;

public:
  //! ctor
  ABasePtrHolder();
  
  //! virtual dtor
  virtual ~ABasePtrHolder();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Insertion of objects
  Associates AObjectBase pointer with name
  
  If ownership is true then the pointer will be deleted in the dtor
  If overwrite is false then exception will be thrown if object already exists
    if ptr is owned and overwritten, the old one will be deleted
  */
  void insert(const AString& name, ABase *, bool ownership = false, bool overwrite = true);
  
  /*!
  Change ownership of an item
  owned items get deleted in the dtor
  */
  void setOwnership(const AString& name, bool ownership = true);

  /*!
  Remove pointer, if owned it will be deleted
  */
  void remove(const AString& name);
  
  /*!
  Clear container and delete owned items
  */
  void clear();

  /*!
  Get pointer stored, NULL if not found
  */
  const ABase *get(const AString& name) const;

  /*!
  Use pointer stored, NULL if not found
  */
  ABase *use(const AString& name);

  /*!
  Usage: const AXmlDocument *pDoc = objects.getAsPtr<AXmlDocument>("xmldoc");
    if xmldoc exists, it will be dynamically cast to AXmlDocument* (in this example) and if types match it will be non-NULL
    NULL returns means either it doesn't exist or it is not of a specified pointer type
  */
  template<typename _type> const _type *getAsPtr(const AString& name) const
  {
    const ABase *ptr = get(name);
    if (ptr)
    {
      _type *pup = dynamic_cast<const _type *>(ptr);
      if (pup)
        return pup;
    }
    
    return NULL;  //a_Either type mismatched or it was not found
  }

  /*!
  Usage: AXmlDocument *pDoc = ns.getAs<AXmlDocument>("xmldoc");
    if xmldoc exists, it will be dynamically cast to AXmlDocument (in this example) and if types match it will be non-NULL
    NULL returns means either it doesn't exist or it is not of a specified pointer type
  */
  template<typename _type> _type *useAsPtr(const AString& name)
  {
    ABase *ptr = use(name);
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
  HOLDER& useContainer();

protected:
  HOLDER m_BasePtrs;

private:
  //! No copy ctor
  ABasePtrHolder(const ABasePtrHolder&) {}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ABasePtrHolder_HPP__
