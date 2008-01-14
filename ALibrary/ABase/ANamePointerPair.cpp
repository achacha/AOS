//
// ANamePointerPair
//

#include "pchABase.hpp"

#include "ANamePointerPair.hpp"
#include "AException.hpp"
#include "AFile.hpp"
#include "templateAutoPtr.hpp"
#include "AOutputBuffer.hpp"

void ANamePointerPair::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(ANamePointerPair @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  //a_Members
  ADebugDumpable::indent(os, indent+1) << "mstr__Name='" << mstr__Name.c_str() << "'";
  ADebugDumpable::indent(os, indent+1) << "mp__Value='" << (u8)mp__Value << "'" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ANamePointerPair::ANamePointerPair(const AString &strName, void *pValue)
{
  if (!strName.isEmpty())
  {
    mstr__Name  = strName; 
    mp__Value = pValue;
  }
}

ANamePointerPair::ANamePointerPair(const ANamePointerPair &nvSource) :
  mstr__Name(nvSource.mstr__Name),
  mp__Value(nvSource.mp__Value)
{
}

void ANamePointerPair::emit(AOutputBuffer& target) const
{
  target.append(mstr__Name);
  target.append('=');
  target.append(AString::fromPointer(mp__Value));
}

void ANamePointerPair::clear()
{  
  //a_Clear internal pair
  mstr__Name.clear();
  mp__Value = NULL;
}

void ANamePointerPair::setNamePointer(const AString &strName, void *pValue)
{
  if (strName.isEmpty())
    ATHROW(this, AException::InvalidParameter);

  mstr__Name = strName;
  mp__Value  = pValue;
}

void ANamePointerPair::setPointer(void *pValue)
{ 
  mp__Value = pValue;
}

int ANamePointerPair::compare(const ANamePointerPair& nvSource) const
{
  int iRet = mstr__Name.compare(nvSource.mstr__Name);
  if (iRet)
    return iRet;

  //a_Pointers compared
  if (mp__Value > nvSource.mp__Value)
    return 0x1;
  else if (mp__Value < nvSource.mp__Value)
    return -0x1;

  return 0x0;
}

bool ANamePointerPair::isName(const AString& str) const
{
  if (mstr__Name.compare(str))
    return false;
  else
    return true;
}

bool ANamePointerPair::isNameNoCase(const AString& str) const
{
  if (mstr__Name.compareNoCase(str))
    return false;
  else
    return true;
}
