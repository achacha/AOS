/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AContentType_Binary.hpp"

const AString AContentType_Binary::CONTENT_TYPE("application/octet-stream",24);

AContentType_Binary::AContentType_Binary() :
  AContentTypeInterface(CONTENT_TYPE)
{ 
}

AContentType_Binary::~AContentType_Binary()
{
}

void AContentType_Binary::parse()
{
}

void AContentType_Binary::emit(AOutputBuffer& target) const
{
  target.append(m_Data);
}
