#include "pchABase.hpp"

#include "ABasePtrHolder.hpp"

void ABasePtrHolder::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABasePtrHolder::ABasePtrHolder()
{
}

ABasePtrHolder::~ABasePtrHolder()
{
}

void ABasePtrHolder::emit(AOutputBuffer& target) const
{
}
