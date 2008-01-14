#include "pchABase.hpp"
#include "AElementObserverInterface.hpp"
#include "AException.hpp"

void AElementObserverInterface::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__peHead=0x" << (void *)m__peHead << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__purlRequest=0x" << m__purlRequest << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AElementObserverInterface::AElementObserverInterface(AElementInterface *pElement, const AUrl *purlRequest) :
  m__peHead(pElement),
  m__purlRequest(purlRequest)
{
  if (purlRequest == NULL)
    ATHROW(this, AException::InvalidParameter);

  if (pElement == NULL)
    ATHROW(this, AException::InvalidParameter);
}

AElementObserverInterface::~AElementObserverInterface()
{
}

