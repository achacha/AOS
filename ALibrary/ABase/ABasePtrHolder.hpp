#ifndef INCLUDED__ABasePtrHolder_HPP__
#define INCLUDED__ABasePtrHolder_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"

/*!
*/
class ABASE_API ABasePtrHolder : public ADebugDumpable
{
public:
  //! ctor
  ABasePtrHolder();
  
  //! virtual dtor
  virtual ~ABasePtrHolder();

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ABasePtrHolder_HPP__
