
#include "pchABase.hpp"
#include "ACache_FileSystem.hpp"
#include "ATime.hpp"
#include "AXmlElement.hpp"
#include "AFile_Physical.hpp"
#include "AFileSystem.hpp"
#include "ALock.hpp"

#ifdef __DEBUG_DUMP__
void ACache_FileSystem::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(ACache_FileSystem @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Hit=" << m_Hit << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Miss=" << m_Miss << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "ATime::getTickCount()=" << ATime::getTickCount() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_CacheArray={" << std::endl;
  for(size_t i=0; i<m_CacheArray.size(); ++i)
  {
    const CONTAINER& container = m_CacheArray.at(i)->m_Cache;
    ADebugDumpable::indent(os, indent+2) << "cache[" << i << "]={" << std::endl;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
    {
      ADebugDumpable::indent(os, indent+3) << "key={" << std::endl;
      (*cit).first.debugDump(os, indent+4);
      ADebugDumpable::indent(os, indent+3) << "}" << std::endl;
      if ((*cit).second)
      {
        ADebugDumpable::indent(os, indent+3) << "value={" << std::endl;
        //a_Turn this on to dump contents, but will be very long
        //(*cit).second->data.debugDump(os, indent+3);
        if ((*cit).second->pData)
          ADebugDumpable::indent(os, indent+4) << "data.size=" << (*cit).second->pData->useAString().getSize() << std::endl;
        else
          ADebugDumpable::indent(os, indent+4) << "data.size=NULL" << std::endl;
        ADebugDumpable::indent(os, indent+3) << "}" << std::endl;
        ADebugDumpable::indent(os, indent+3) << "hits=" << (*cit).second->hits << std::endl;
        ADebugDumpable::indent(os, indent+3) << "lastUsed=" << (*cit).second->lastUsed << std::endl;
      }
      else
      {
        ADebugDumpable::indent(os, indent+3) << "value=NULL" << std::endl;
      }
    }
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

ACache_FileSystem::ACache_FileSystem(size_t hashSize, size_t maxItems) :
  m_MaxItems(maxItems),
  m_Hit(0),
  m_Miss(0)
{
  m_CacheArray.resize(hashSize);
  for (size_t i=0; i<hashSize; ++i)
  {
    m_CacheArray[i] = new CONTAINER_ITEM();
  }
}

ACache_FileSystem::~ACache_FileSystem()
{
  try
  {
    for (size_t i=0; i<m_CacheArray.size(); ++i)
      delete m_CacheArray.at(i);
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

ACache_FileSystem::CONTAINER_ITEM::CONTAINER_ITEM()
{
}

ACache_FileSystem::CONTAINER_ITEM::~CONTAINER_ITEM()
{
  //a_Release memory from the cache
  try
  {
    for (CONTAINER::iterator it = m_Cache.begin(); it != m_Cache.end(); ++it)
      delete (*it).second;
  }
  catch(...) {}
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

  for (size_t i = 0; i < m_CacheArray.size(); ++i)
  {
    AXmlElement& eCache = base.addElement(ASW("cache",5));
    const CONTAINER_ITEM *pCache = m_CacheArray.at(i);
    eCache.addAttribute(ASW("id",2), AString::fromSize_t(i));

    const CONTAINER& container = pCache->m_Cache;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
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

  for (size_t i = 0; i < m_CacheArray.size(); ++i)
  {
    const CONTAINER_ITEM *pCache = m_CacheArray.at(i);
    target.append("---CACHE[",9);
    target.append(AString::fromSize_t(i));
    target.append("]",1);

    const CONTAINER& container = pCache->m_Cache;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
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
}

AFile *ACache_FileSystem::get(const AFilename& key)
{
  //TODO:
  const AString& strKey = key.toAString();
  size_t hash = strKey.getHash(m_CacheArray.size());
  CONTAINER& container = m_CacheArray.at(hash)->m_Cache;

  CONTAINER::iterator it = container.find(strKey);
  if (it == container.end())
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

      container[strKey] = p.get();
      p.setOwnership(false);
      return p->pData;
    }
    else
    {
      //a_File is a directory or does not exist, NULL value for p->pData
      CACHE_ITEM *p = new CACHE_ITEM();
      container[strKey] = p;
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
  size_t size=0;
  for (size_t i=0; i<m_CacheArray.size(); ++i)
  {
    size += m_CacheArray.at(i)->m_Cache.size();
  }
  return size;
}

size_t ACache_FileSystem::getHit() const
{
  return m_Hit;
}

size_t ACache_FileSystem::getMiss() const
{
  return m_Miss;
}
