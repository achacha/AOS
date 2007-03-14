#ifndef INCLUDED__ACache_HPP__
#define INCLUDED__ACache_HPP__

#include "apiABase.hpp"
#include "AStringHashMap.hpp"

class ABASE_API ACache : public AStringHashMap
{
public:
  /*
  Create a cache using a synchronization object (OWNED and DELETED by this class)

  Usage:
    ACache mycache(new AMutex("mycachemutex"));

  */
  ACache(ASynchronization *pSynchObject);
  virtual ~ACache();

  /*!
  AEmittable
  AXmlEmittable
  */
//  void emit(AXmlElement& target) const;
//  void emit(AEmittable& target) const;

protected:


private:
  ACache() {}
  ACache(const ACache&) {}

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ACache_HPP__
