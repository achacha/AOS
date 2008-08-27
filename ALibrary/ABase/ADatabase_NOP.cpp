/*
Written by Alex Chachanashvili

$Id: ADatabase_NOP.cpp 226 2008-07-24 21:08:40Z achacha $
*/
#include "pchABase.hpp"
#include "ADatabase_NOP.hpp"

ADatabase_NOP::ADatabase_NOP()
{
}

ADatabase_NOP::ADatabase_NOP(const AUrl& urlConnection) :
  ADatabase(urlConnection)
{
}

ADatabase_NOP::~ADatabase_NOP()
{
}

bool ADatabase_NOP::init(AString& error)
{
  mbool_Initialized = true;
  return true;
}

void ADatabase_NOP::deinit()
{
}

bool ADatabase_NOP::reconnect(AString& error)
{
  return true;
}

size_t ADatabase_NOP::executeSQL(const AString& query, AResultSet&, AString& error)
{
  return 0;
}

ADatabase_NOP *ADatabase_NOP::clone(AString& error) const
{
  return new ADatabase_NOP();
}
