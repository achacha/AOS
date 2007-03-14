
#include "pchABase.hpp"
#include "ASynchronization.hpp"

#ifdef __DEBUG_DUMP__
void ASynchronization::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
}
#endif

ASynchronization::ASynchronization(eInitialState /* i = UNLOCKED */)
{
}

ASynchronization::~ASynchronization()
{
}


