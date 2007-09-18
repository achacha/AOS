
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
  ADebugDumpable::indent(os, indent+1) << "m_MaxFileSize=" << m_MaxFileSize << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_MaxItems=" << m_MaxItems << std::endl;

  ADebugDumpable::indent(os, indent+1) << "ATime::getTickCount()=" << ATime::getTickCount() << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_CacheArray={" << std::endl;
  for(size_t i=0; i<m_CacheArray.size(); ++i)
  {
    const CONTAINER& container = m_CacheArray.at(i)->m_Cache;
    ADebugDumpable::indent(os, indent+2) << "cache[" << i << "]={" << std::endl;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
    {
      ADebugDumpable::indent(os, indent+3) << "entry={" << (*cit).first << std::endl;
      ADebugDumpable::indent(os, indent+4) << "key=" << (*cit).first << std::endl;
      if ((*cit).second)
      {
        ADebugDumpable::indent(os, indent+4) << "value={" << std::endl;
        //a_Turn this on to dump contents, but will be very long
        //(*cit).second->data.debugDump(os, indent+3);
        if ((*cit).second->pData)
          ADebugDumpable::indent(os, indent+5) << "data.size=" << (*cit).second->pData->useAString().getSize() << std::endl;
        else
          ADebugDumpable::indent(os, indent+5) << "data.size=NULL" << std::endl;
        ADebugDumpable::indent(os, indent+4) << "}" << std::endl;
        ADebugDumpable::indent(os, indent+4) << "hits=" << (*cit).second->hits << std::endl;
        ADebugDumpable::indent(os, indent+4) << "lastUsed=" << (*cit).second->lastUsed << std::endl;
        ADebugDumpable::indent(os, indent+3) << "}" << std::endl;
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

ACache_FileSystem::ACache_FileSystem(size_t maxItems, size_t maxFileSize, size_t hashSize) :
  m_MaxItems(maxItems),
  m_MaxFileSize(maxFileSize),
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
  lastModified(0),
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

ACache_FileSystem::CONTAINER_ITEM::CONTAINER_ITEM() :
  m_ByteSize(0)
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

void ACache_FileSystem::CONTAINER_ITEM::clear()
{
  ALock lock(m_Sync);
  for (CONTAINER::iterator it = m_Cache.begin(); it != m_Cache.end(); ++it)
    delete (*it).second;
  m_Cache.clear();
  m_ByteSize = 0;
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
  const AString COUNT("count", 5);
  const AString BYTES("bytes", 5);

  AXmlElement& base = target.addElement(ASW("ACache_FileSystem",17));
  base.addAttribute(HITS, AString::fromSize_t(m_Hit));
  base.addAttribute(ASW("miss",4), AString::fromSize_t(m_Miss));
  base.addAttribute(ASW("max_items",9), AString::fromSize_t(m_MaxItems));
  base.addAttribute(ASW("max_filesize",12), AString::fromSize_t(m_MaxFileSize));
  base.addAttribute(ASW("time_now",8), AString::fromSize_t(ATime::getTickCount()));

  for (size_t i = 0; i < m_CacheArray.size(); ++i)
  {
    AXmlElement& eCache = base.addElement(ASW("cache",5));
    const CONTAINER_ITEM *pCache = m_CacheArray.at(i);
    eCache.addAttribute(ASW("id",2), AString::fromSize_t(i));

    eCache.addAttribute(COUNT, AString::fromSize_t(pCache->m_Cache.size()));
    eCache.addAttribute(BYTES, AString::fromSize_t(pCache->m_ByteSize));

    const CONTAINER& container = pCache->m_Cache;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
    {
      AXmlElement& file = base.addElement(KEY, (*cit).first, AXmlData::None, false);  //a_Create new key instead of reusing
      file.addAttribute(HITS, AString::fromInt((*cit).second->hits));
      file.addAttribute(LAST, AString::fromS8((*cit).second->lastUsed));
      file.addAttribute(FOUND, (NULL == (*cit).second->pData ? AConstant::ASTRING_FALSE : AConstant::ASTRING_TRUE));
    }
  }
}

void ACache_FileSystem::emit(AOutputBuffer& target) const
{
  target.append("MaxItems=",9);
  target.append(AString::fromSize_t(m_MaxItems));
  target.append("  MaxFileSize=",14);
  target.append(AString::fromSize_t(m_MaxFileSize));
  target.append("  H/M=",6);
  target.append(AString::fromSize_t(m_Hit));
  target.append('/');
  target.append(AString::fromSize_t(m_Miss));
  target.append("  timeNow=",10);
  target.append(AString::fromSize_t(ATime::getTickCount()));
  target.append(AConstant::ASTRING_EOL);

  for (size_t i = 0; i < m_CacheArray.size(); ++i)
  {
    const CONTAINER_ITEM *pCache = m_CacheArray.at(i);
    target.append("---CACHE[",9);
    target.append(AString::fromSize_t(i));
    target.append("]",1);
    target.append("(count=",7);
    target.append(AString::fromSize_t(pCache->m_Cache.size()));
    target.append("  bytes=",8);
    target.append(AString::fromSize_t(pCache->m_ByteSize));
    target.append(")\r\n",3);

    const CONTAINER& container = pCache->m_Cache;
    for(CONTAINER::const_iterator cit = container.begin(); cit != container.end(); ++cit)
    {
      target.append("  ",2);
      target.append((*cit).first);
      target.append(" (",2);
      target.append((*cit).second->pData ? AString::fromSize_t((*cit).second->pData->useAString().getSize()) : ASW("-",1));
      target.append(") [",3);
      target.append(AString::fromInt((*cit).second->hits));
      target.append(']');
      target.append(" last=",6);
      target.append(AString::fromS8((*cit).second->lastUsed));
      target.append(AConstant::ASTRING_EOL);
    }
  }
}

ACache_FileSystem::STATUS ACache_FileSystem::get(
  const AFilename& key, 
  AAutoPtr<AFile>& pFile, 
  ATime& modified,
  const ATime& ifModifiedSince   // = ATime::GENESIS
)
{
  const AString& strKey = key.toAString();
  size_t hash = strKey.getHash(m_CacheArray.size());
  CONTAINER_ITEM& containerItem = *(m_CacheArray.at(hash));

  CONTAINER::iterator it = containerItem.m_Cache.find(strKey);
  if (it == containerItem.m_Cache.end())
  {
    ++m_Miss;

    //a_Read in data
    ALock lock(containerItem.m_Sync);
    if (AFileSystem::isA(key, AFileSystem::File))
    {
      if (AFileSystem::length(key) > m_MaxFileSize)
      {
        //a_File too big to cache, create a physical object and return it with auto delete on
        pFile.reset(new AFile_Physical(key, "rb"));
        pFile->open();
        modified.setToNow();
        return ACacheInterface::FOUND_NOT_CACHED;
      }
      else
      {
        //a_File exists cache it
        AAutoPtr<CACHE_ITEM> p(new CACHE_ITEM());
        p->pData = new AFile_AString((size_t)AFileSystem::length(key), 256);
        
        if (!AFileSystem::getLastModifiedTime(key, p->lastModified))
          ATHROW_EX(this, AException::NotFound, key);

        AFile_Physical file(key, "rb");
        file.open();
        file.readUntilEOF(p->pData->useAString());

        //a_Add/set new data and add to byte count
        containerItem.m_Cache[strKey] = p.get();
        containerItem.m_ByteSize += p.get()->pData->getAString().getSize();
        p.setOwnership(false);
        
        pFile.reset(p->pData, false);
        modified = p->lastModified;

        AASSERT(this, containerItem.m_Cache.size() < DEBUG_MAXSIZE_ACache_FileSystem);  //Debug only limit
        return ACacheInterface::FOUND;
      }
    }
    else
    {
      //a_File is a directory or does not exist, NULL value for p->pData
      CACHE_ITEM *p = new CACHE_ITEM();
      containerItem.m_Cache[strKey] = p;

      pFile.reset(NULL);
      modified = ATime::GENESIS;

      AASSERT(this, containerItem.m_Cache.size() < DEBUG_MAXSIZE_ACache_FileSystem);  //Debug only limit
      return ACacheInterface::NOT_FOUND;
    }
  }
  else
  {
    //a_Item found in cache
    ++m_Hit;
    (*it).second->hit();
    
    pFile.reset(it->second->pData, false);
    modified = it->second->lastModified;

    if (modified > ifModifiedSince)
      return ACacheInterface::FOUND;
    else
      return ACacheInterface::FOUND_NOT_MODIFIED;
  }
}

size_t ACache_FileSystem::getItemCount() const
{
  size_t size=0;
  for (size_t i=0; i<m_CacheArray.size(); ++i)
  {
    size += m_CacheArray.at(i)->m_Cache.size();
  }
  return size;
}

size_t ACache_FileSystem::getByteSize() const
{
  size_t size=0;
  for (size_t i=0; i<m_CacheArray.size(); ++i)
  {
    size += m_CacheArray.at(i)->m_ByteSize;
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

void ACache_FileSystem::clear()
{
  for (size_t i=0; i<m_CacheArray.size(); ++i)
  {
    CONTAINER_ITEM *pContainerItem = m_CacheArray.at(i);
    pContainerItem->clear();
  }
  m_Hit = 0;
  m_Miss = 0;
}
