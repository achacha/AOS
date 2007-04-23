
#include "pchABase.hpp"
#include "ACache_FileSystem.hpp"
#include "ATime.hpp"
#include "AXmlElement.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ASync_CriticalSection.hpp"
#include "ALock.hpp"

#ifdef __DEBUG_DUMP__
void ACache_FileSystem::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ACache_FileSystem @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Hit=" << m_Hit << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Miss=" << m_Miss << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "ATime::getTickCount()=" << ATime::getTickCount() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Cache={" << std::endl;
  for(CONTAINER::const_iterator cit = m_Cache.begin(); cit != m_Cache.end(); ++cit)
  {
    ADebugDumpable::indent(os, indent+2) << "key={" << std::endl;
    (*cit).first.debugDump(os, indent+3);
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    if ((*cit).second)
    {
      ADebugDumpable::indent(os, indent+2) << "value={" << std::endl;
      //a_Turn this on to dump contents, but will be very long
      //(*cit).second->data.debugDump(os, indent+3);
      if ((*cit).second->pData)
        ADebugDumpable::indent(os, indent+3) << "data.size=" << (*cit).second->pData->useAString().getSize() << std::endl;
      else
        ADebugDumpable::indent(os, indent+3) << "data.size=NULL" << std::endl;
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
      ADebugDumpable::indent(os, indent+2) << "hits=" << (*cit).second->hits << std::endl;
      ADebugDumpable::indent(os, indent+2) << "lastUsed=" << (*cit).second->lastUsed << std::endl;
    }
    else
    {
      ADebugDumpable::indent(os, indent+2) << "value=NULL" << std::endl;
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ACache_FileSystem::ACache_FileSystem(size_t maxItems) :
  m_MaxItems(maxItems),
  m_Hit(0),
  m_Miss(0)
{
  mp_Sync = new ASync_CriticalSection();
}

ACache_FileSystem::~ACache_FileSystem()
{
  try
  {
    delete mp_Sync;
    for(CONTAINER::iterator it = m_Cache.begin(); it != m_Cache.end(); ++it)
      delete (*it).second->pData;
  }
  catch(...) {}
}

ACache_FileSystem::CACHE_ITEM::CACHE_ITEM():
  pData(NULL),
  hits(0),
  lastUsed(ATime::getTickCount())
{
}

ACache_FileSystem::CACHE_ITEM::~CACHE_ITEM()
{
  delete(pData);
}

void ACache_FileSystem::CACHE_ITEM::hit()
{
  ++hits;
  lastUsed = ATime::getTickCount();
}

void ACache_FileSystem::manage()
{
}

void ACache_FileSystem::emit(AXmlElement& target) const
{
  const AString KEY("key", 3);
  const AString HITS("hits", 4);
  const AString LAST("last_used", 9);
  const AString FOUND("found", 5);
  const AString SIZE("size", 4);

  AXmlElement& base = target.addElement(ASW("ACache_FileSystem",17));
  base.addAttribute(HITS, AString::fromSize_t(m_Hit));
  base.addAttribute(ASW("miss",4), AString::fromSize_t(m_Miss));
  base.addAttribute(ASW("max_items",9), AString::fromSize_t(m_MaxItems));
  base.addAttribute(ASW("time_now",8), AString::fromSize_t(ATime::getTickCount()));

  for(CONTAINER::const_iterator cit = m_Cache.begin(); cit != m_Cache.end(); ++cit)
  {
    AXmlElement& file = base.addElement(KEY, (*cit).first, AXmlData::None, false);  //a_Create new key instead of reusing
    file.addAttribute(HITS, AString::fromInt((*cit).second->hits));
    file.addAttribute(LAST, AString::fromS8((*cit).second->lastUsed));
    if (NULL == (*cit).second->pData)
    {
      file.addAttribute(FOUND, AString::sstr_False);
    }
    else
    {
      file.addAttribute(SIZE, AString::fromSize_t((*cit).second->pData->useAString().getSize()));
    }
  }
}

void ACache_FileSystem::emit(AOutputBuffer& target) const
{
  target.append("maxItems=",9);
  target.append(AString::fromSize_t(m_MaxItems));
  target.append("  H/M=",6);
  target.append(AString::fromSize_t(m_Hit));
  target.append('/');
  target.append(AString::fromSize_t(m_Miss));
  target.append("  timeNow=",10);
  target.append(AString::fromSize_t(ATime::getTickCount()));
  target.append(AString::sstr_EOL);

  for(CONTAINER::const_iterator cit = m_Cache.begin(); cit != m_Cache.end(); ++cit)
  {
    target.append((*cit).first);
    target.append(" (",2);
    target.append((*cit).second->pData ? AString::fromSize_t((*cit).second->pData->useAString().getSize()) : ASW("-",1));
    target.append(") [",3);
    target.append(AString::fromInt((*cit).second->hits));
    target.append(']');
    target.append(" last=",6);
    target.append(AString::fromS8((*cit).second->lastUsed));
    target.append(AString::sstr_EOL);
  }
}

AFile *ACache_FileSystem::get(const AFilename& key)
{
  ALock lock(mp_Sync);
  CONTAINER::iterator it = m_Cache.find(key);
  if (it == m_Cache.end())
  {
    ++m_Miss;

    //a_Read in data
    if (AFileSystem::exists(key) && AFileSystem::isA(key, AFileSystem::File))
    {
      AAutoPtr<CACHE_ITEM> p(new CACHE_ITEM());
      p->pData = new AFile_AString((size_t)AFileSystem::length(key), 256);

      AFile_Physical file(key, "rb");
      file.open();
      file.readUntilEOF(p->pData->useAString());

      m_Cache[key] = p.get();
      p.setOwnership(false);
      return p->pData;
    }
    else
    {
      //a_File is a directory or does not exist, NULL value for p->pData
      CACHE_ITEM *p = new CACHE_ITEM();
      m_Cache[key] = p;
      return NULL;
    }
  }
  else
  {
    ++m_Hit;
    (*it).second->hit();
    return (*it).second->pData;
  }
}

size_t ACache_FileSystem::getSize() const
{
  return m_Cache.size();
}
