#ifndef INCLUDED__AFilePeekable_HPP_
#define INCLUDED__AFilePeekable_HPP_

#include "apiABase.hpp"
#include "AFile.hpp"
#include "AFilePeekable.hpp"

/*!
AFile type that can be random peeked
*/
class ABASE_API AFilePeekable : public AFile, public APeekable
{
public:
  virtual ~AFilePeekable();
};

#endif //INCLUDED__AFilePeekable_HPP_
