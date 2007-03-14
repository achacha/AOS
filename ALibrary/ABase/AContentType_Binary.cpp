#include "pchABase.hpp"
#include "AContentType_Binary.hpp"
#include "ATextConverter.hpp"

AContentType_Binary::AContentType_Binary()
{ 
}

void AContentType_Binary::emit(AOutputBuffer& target, size_t /* indent = AConstant::npos */) const
{
  ATextConverter::convertStringToHexDump(mstr_Data, target);
}
