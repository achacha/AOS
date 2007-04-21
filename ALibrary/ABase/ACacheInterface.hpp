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
  Cache management function, should be called by cache manager thread/loop
  */
  virtual void manage() = 0; 
};

#endif // INCLUDED__ACacheInterface_HPP__
