#ifndef INCLUDED__ACacheInterface_HPP__
#define INCLUDED__ACacheInterface_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AEmittable.hpp"
#include "AXmlEmittable.hpp"

/*!
Maps AFilename -> AString (which contains contents of the AFilename or whatever else)
*/
class ABASE_API ACacheInterface : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  ctor
  */
  ACacheInterface();

  /*!
  dtor
  */
  virtual ~ACacheInterface();

  /*!
  Status
  */
  enum STATUS
  {
    NOT_FOUND = 0,
    FOUND = 1,
    FOUND_NOT_CACHED = 2,
    FOUND_NOT_MODIFIED = 3
  };

  /*!
  Cache management function, should be called by cache manager thread/loop
  */
  virtual void manage() = 0; 

  /*!
  Clear the cache
  */
  virtual void clear() = 0;
};

#endif // INCLUDED__ACacheInterface_HPP__
