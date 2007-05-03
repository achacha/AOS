
#include "apiABase.hpp"
#include "ACacheInterface.hpp"
#include "AFilename.hpp"
#include "AFile_AString.hpp"

class ASynchronization;

class ABASE_API ACache_FileSystem : public ACacheInterface
{
public:
  /*!
  ctor
  */
  ACache_FileSystem(size_t maxItems = 10240);

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
  Cache item count
  */
  size_t getSize() const;

private:
  size_t m_MaxItems;

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
  CONTAINER m_Cache;

  size_t m_Hit;
  size_t m_Miss;

  ASynchronization *mp_Sync;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};
