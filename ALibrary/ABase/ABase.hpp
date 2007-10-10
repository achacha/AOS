#ifndef INCLUDED__ABase_hpp__
#define INCLUDED__ABase_hpp__

#include "apiABase.hpp"

/*!
Simple common base class

Allows use of dynamic_cast<> since void* is not allowed then
*/
class ABASE_API ABase
{
public:
  virtual ~ABase();
};

#endif // INCLUDED__ABase_hpp__

