
#include "apiABase.hpp"
#include "ACacheInterface.hpp"
#include "AFilename.hpp"
#include "AFile_AString.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"
#include "templateAutoPtr.hpp"
#include "ATime.hpp"

class ABASE_API ACache_FileSystem : public ACacheInterface
{
public:
  /*!
  ctor

  maxItems to store in cache
  maxFileSize in bytes that are cached

  hashSize MUST be prime
  */
  ACache_FileSystem(size_t maxItems = 10240, size_t maxFileSize = 1048576, size_t hashSize = 97);

  /*!
  dtor
  */
  virtual ~ACache_FileSystem();

  /*!
  AEmittable, AXmlEmittable
  */
  virtual void emitXml(AXmlElement&) const;
  virtual void emit(AOutputBuffer&) const;
  
  /*!
  ACacheInterface
  */
  virtual void manage();

  /*!
  Create item if it does not exist
  If does not exist, will read the file and cache it
  
  pFile is set to AFile * that is either a cached one or from disk if file is too large to cache
    auto-delete is set accordingly and item handles (or not) when AAutoPtr is out of scope
  */
  ACacheInterface::STATUS get(
    const AFilename& key,                             // Filename to find
    AAutoPtr<AFile>& pFile,                           // Set to AFile in the cache
    ATime& modified,                                  // Set to modified date of the file (the one in cache)
    const ATime& ifModifiedSince = ATime::GENESIS     // Controls the return status to include FOUND_NOT_MODIFIED if applies
  );

  /*!
  Cache item count (total)
  */
  size_t getItemCount() const;

  /*!
  Cache item size (total)
  */
  size_t getByteSize() const;

  /*!
  Get hit count
  */
  size_t getHit() const;

  /*!
  Get miss count
  */
  size_t getMiss() const;

  /*!
  Clear the cache
  */
  virtual void clear();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  size_t m_MaxItems;
  size_t m_MaxFileSize;

  //a_Item container inside the cache
  struct CACHE_ITEM
  {
    AFile_AString *pData;
    int hits;
    ATime lastUsed;
    ATime lastModified;

    CACHE_ITEM();
    ~CACHE_ITEM();
    void hit();
  };

  typedef std::map<AString, CACHE_ITEM *> CONTAINER;
  
  //a_Contains the cache, sync, byte count, etc
  struct CONTAINER_ITEM
  {
    ASync_CriticalSectionSpinLock m_Sync;
    CONTAINER m_Cache;

    size_t m_ByteSize;  //total bytes in this cache

    CONTAINER_ITEM();
    ~CONTAINER_ITEM();

    void clear();
  };

  typedef std::vector<CONTAINER_ITEM *> CONTAINERS;
  CONTAINERS m_CacheArray;

  size_t m_Hit;
  size_t m_Miss;

  ASync_CriticalSectionSpinLock m_Sync;
};
