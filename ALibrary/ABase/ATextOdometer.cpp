/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATextOdometer.hpp"
#include "ATextGenerator.hpp"

void ATextOdometer::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "mstr_Odometer=" << mstr_Odometer.c_str() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mstr_Subset=" << mstr_Subset.c_str() << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATextOdometer::ATextOdometer(size_t iSize, const AString& strSubset) :
  mstr_Subset(strSubset)
{
  //a_Create the initial string
  clear(iSize);
}

ATextOdometer::ATextOdometer(const AString& initial, size_t iSize, const AString& strSubset) :
  mstr_Subset(strSubset)
{
  //a_Create the initial string
  clear(iSize);
  setInitial(initial);
}

ATextOdometer::ATextOdometer(const ATextOdometer& that)
{
  mstr_Odometer = that.mstr_Odometer;
  mstr_Subset = that.mstr_Subset;
}

ATextOdometer::~ATextOdometer()
{
}

void ATextOdometer::clear(
  size_t size // = AConstant::npos
)
{
  if (AConstant::npos == size)
    size = mstr_Odometer.getSize();

  mstr_Odometer.clear();
  mstr_Odometer.setSize(size, mstr_Subset.at(0));
}

void ATextOdometer::setSubset(const AString& strSubset)
{
  AASSERT(this, strSubset.getSize() > 0);
  mstr_Subset = strSubset;
  clear(strSubset.getSize());
}

const AString& ATextOdometer::setInitial(const AString& strInitial)
{
  //a_Initial must be the same or smaller in size as odometer
  AASSERT(this, strInitial.getSize() <= mstr_Odometer.getSize());

  size_t iI = strInitial.getSize();
  for (size_t iX = mstr_Odometer.getSize(); iX > 0; --iX)
  {
    if (iI)
    {
      AASSERT(this, AConstant::npos != mstr_Subset.find(strInitial.at(iI-1)));
      mstr_Odometer.set(strInitial.at(iI-1), iX-1);
      --iI;
    }
    else
      mstr_Odometer.set(mstr_Subset.at(0), iX-1);
  }

  return mstr_Odometer;
}

const AString& ATextOdometer::setRandomInitial()
{
  size_t sizeToGen = mstr_Odometer.getSize();
  mstr_Odometer.clear();
  ATextGenerator::generateRandomString(mstr_Odometer, sizeToGen, mstr_Subset);

  return mstr_Odometer;
}

const ATextOdometer& ATextOdometer::operator ++(void)
{ 
  _inc();
  return *this;
}

const ATextOdometer& ATextOdometer::operator --(void)
{
  _dec();
  return *this;
}

const ATextOdometer ATextOdometer::operator ++(int)
{
  ATextOdometer cRet(*this);
  _inc();
  return cRet;
}

const ATextOdometer ATextOdometer::operator --(int)
{
  ATextOdometer cRet(*this);
  _dec();
  return cRet;
}

bool ATextOdometer::operator ==(const ATextOdometer& source) const
{
  if ((this == &source) || (!source.mstr_Odometer.compare(mstr_Odometer)) )
    return true;
  else
    return false;
}

bool ATextOdometer::operator !=(const ATextOdometer& source) const
{ 
  if (this == &source)
    return true;
  else
    return !operator==(source);
}

const AString& ATextOdometer::get() const
{
  return mstr_Odometer;
}

void ATextOdometer::emit(AOutputBuffer& target) const
{
  target.append(mstr_Odometer);
}

void ATextOdometer::_inc()
{
  size_t s = 0;
  size_t pos = mstr_Odometer.getSize();
  AASSERT(this, pos);
	pos--;

  for (size_t i = pos; i != (size_t)-1; --i)
  {
    //a_Find the location and increment
    s = mstr_Subset.find(mstr_Odometer.at(i));
    if ((s + 0x1) < mstr_Subset.getSize())
    {
      mstr_Odometer.set(mstr_Subset.at(++s), i);
      break;
    }
    else
      mstr_Odometer.set(mstr_Subset.at(0x0), i);
  }
}

void ATextOdometer::_dec()
{
  size_t s = 0;
  size_t pos = mstr_Odometer.getSize();
  AASSERT(this, pos);
	pos--;

  for (size_t i = pos; i != AConstant::npos; --i)
  {
    //a_Find the location and increment
    s = mstr_Subset.find(mstr_Odometer.at(i));
    if (s > 0x0)
    {
      mstr_Odometer.set(mstr_Subset.at(--s), i);
      break;
    }
    else
      mstr_Odometer.set(mstr_Subset.at(mstr_Subset.getSize() - 1), i);
  }
}

size_t ATextOdometer::getSize() const
{
  return mstr_Odometer.getSize();
}

const AString& ATextOdometer::getSubset() const
{
  return mstr_Subset;
}