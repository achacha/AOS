#include "pchABase.hpp"
#include "ABasePtrQueue.hpp"

void ABasePtrQueue::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  //ADebugDumpable::indent(os, indent+1) << "myname=" << m_name << std::endl;      //TODO:
  //ADebugDumpable::indent(os, indent+1) << "myvalue=" << m_value << std::endl;    //TODO:
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ABasePtrQueue::ABasePtrQueue()
{
}

ABasePtrQueue::~ABasePtrQueue()
{
}

void ABasePtrQueue::pushFront(ABase *p)
{
}

ABase *ABasePtrQueue::popFront()
{
  return NULL;
}

void ABasePtrQueue::pushBack(ABase *p)
{
}

ABase *ABasePtrQueue::popBack()
{
  return NULL;
}
