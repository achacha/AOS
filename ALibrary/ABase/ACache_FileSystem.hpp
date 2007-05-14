
#include "apiABase.hpp"
#include "ACacheInterface.hpp"
#include "AFilename.hpp"
#include "AFile_AString.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"

class ABASE_API ACache_FileSystem : public ACacheInterface
{
public:
  /*!
  ctor
  */
  ACache_FileSystem(size_t maxItems = 10240, size_t hashSize = 97);

  /*!
  dtor
  */
  virtual ~ACache_FileSystem();

  /*!
  AEmittable, AXmlEmittable
  */
  virtual void emit(AXmlElement&) const;
  virtual void emit(AOutputBuffer&) const;
  
  /*!
  ACacheInterface
  */
  virtual void manage();

  /*!
  Create item if it does not exist
  If does not exist, will read the file and cache it
  Returns AFile * which is data object for a given key filename
  Returns NULL if file is not found
  */
  AFile *get(const AFilename& key);

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

private:
  size_t m_MaxItems;

  //a_Item container inside the cache
  struct CACHE_ITEM
  {
    AFile_AString *pData;
    int hits;
    s8 lastUsed;

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

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};
