#ifndef INCLUDED__AObjectBase_HPP__
#define INCLUDED__AObjectBase_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"
#include "AXmlAttributes.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "AXmlEmittable.hpp"

class AFile;

/*!
 Public methods that must be implemented by every child class:

public:
//REQUIRED: AEmittable and AXmlEmittable
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;

//OPTIONAL: Debugging to ostream (optional, but nice to have)
	virtual void debugDump(std::ostream&, int iIndentLevel = 0x0) const;

//REQUIRED: can use this as is without change, will use copy ctor to clone, just put your class name instead of MYCLASS
  virtual AObjectBase* clone() const { return new MYCLASS(*this); }  //a_Clone self using copy ctor (this can just be copied as is)

*/
class ABASE_API AObjectBase : 
  public ASynchronization, 
  public AXmlEmittable
{
public:
  /*!
  name - name of the current object
  NOTE: AObjectContainer can override this if it is explicitly specified during insertion
  NOTE: Copy ctor will NOT copy the target synchronization to avoid locking problems, after copy a new synch object needs to be set
  */
  AObjectBase();
  AObjectBase(const AString& name);
  AObjectBase(const AObjectBase&);
  virtual ~AObjectBase();
  
  /*!
  Clone self
  NOTE: Cloned object does not copy the synchronization associated with the original after copy the new object is unsynchronized
  */
  virtual AObjectBase* clone() const = 0;

  /*!
  Name
  */
  const AString& getName() const { return m_Name; }
  void setName(const AString& name) { m_Name = name; }

  /*!
  Attributes
  */
  AXmlAttributes& useAttributes() { return m_Attributes; }

  /*!
  Synchronization to wrap by this object
   Any changes should be done via ALock to prevent leaving locked objects around
   - NULL will force a delete of the existing synchronization
   - the object will own the synchronization and will delete it when destroyed
   - see lock/unlock/trylock/canlock below on how it can be used in any derived class
  NOTE: This object will OWN and DELETE this pointer, so you should use: myObject.setSynchronization(new ASync_Mutex())
  */
  void setSynchronization(ASynchronization *pSynchObject);

  /*!
  Overrides for ASynchronization (they pass call thru to the synchronizatin object if any)
  If you do not have any synch object it will do nothing
   Usage example:
     class MyObject: public AObjectBase
     {
     ...
     };

     AMyObject foo("someobject");
     foo.setSynchonization(new ASync_CriticalSection());
  
     {
       // Use of { } will guarantee that when ALock object goes out of scope it unlocks the synch object automatically
       ALock lock(foo);
       foo.performSomeSerializedTask();
     }
  */
  virtual void lock();         //a_Lock this object
  virtual bool trylock();      //a_Try and lock, return status
  virtual void unlock();       //a_Unlock this object

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //a_Name of this object
  AString m_Name;
  
  //a_Attributes of this object
  AXmlAttributes m_Attributes;
  
  //a_Access synchronization (if any)
  ASynchronization *mp_SynchObject;
};

#endif //INCLUDED__AObjectBase_HPP__
