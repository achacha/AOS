/*
Written by Alex Chachanashvili

$Id: AOSSessionManager.hpp 252 2008-08-02 21:26:15Z achacha $
*/
#ifndef INCLUDED__AOSSessionMapHolder_HPP__
#define INCLUDED__AOSSessionMapHolder_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "ASynchronization.hpp"
#include "AOSSessionData.hpp"

/*!
Session map holder
*/
class AOS_BASE_API AOSSessionMapHolder : public ADebugDumpable
{
public:
  //! Map of session id to session data
  typedef std::map<AString, AOSSessionData *> CONTAINER;

public:
  //! ctor
  AOSSessionMapHolder(ASynchronization *pSynch);
  
  //! dtor
  virtual ~AOSSessionMapHolder();

  bool exists(const AString& sessionId);
  
  /*!
  Gets an existing session or creates a new one for the provided id

  @param sessionId to look up
  @return AOSSessionData pointer (DO NOT DELETE)
  */
  AOSSessionData *getOrCreateSessionData(const AString& sessionId);

  /*!
  Purse data that has expired based on timeout interval
  
  @param timeoutInterval to use for expiration
  */
  void purgeExpired(double timeoutInterval);

  /*!
  Access the container of AOSSessionData pointers
  */
  AOSSessionMapHolder::CONTAINER useContainer();

  /*!
  Set session data from serialized string

  @param sessionId to associate this data with
  @param data to use with AOSSessionData.fromAFile
  */
  void setSessionData(const AString& sessionId, const AString& data);

  /*!
  Access the sync object
  */
  ASynchronization& useSyncObject();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! AOSSessionData container
  AOSSessionMapHolder::CONTAINER m_SessionMap;
  
  //! Sync for this container
  ASynchronization *mp_SyncObject;
};

#endif //INCLUDED__AOSSessionMapHolder_HPP__
