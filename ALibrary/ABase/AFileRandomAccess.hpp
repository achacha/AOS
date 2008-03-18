#ifndef INCLUDED__AFileRandomAccess_HPP_
#define INCLUDED__AFileRandomAccess_HPP_

#include "apiABase.hpp"
#include "AFile.hpp"
#include "ARandomAccessBuffer.hpp"

/*!
AFile type that can be random accessed
*/
class ABASE_API AFileRandomAccess : public AFile, public ARandomAccessBuffer
{
public:
  virtual ~AFileRandomAccess();
};

#endif //INCLUDED__AFileRandomAccess_HPP_
