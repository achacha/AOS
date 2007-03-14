#ifndef INCLUDED__AOSRequest_HPP__
#define INCLUDED__AOSRequest_HPP__

#include "apiAOS_Base.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "ATimer.hpp"
#include "ARopeDeque.hpp"

class AFile_Socket;
class AOSServices;

class AOS_BASE_API AOSRequest : public ADebugDumpable, public AXmlEmittable
{
public:
  /*!
  Allocation and deallocation of AOSRequest object
  */
  static AOSRequest *allocate(AOSServices&, AFile_Socket *);
  static void deallocate(AOSRequest *);

  /*!
  AOSRequest owns the AFile object and will delete it in dtro
  */
  virtual ~AOSRequest();

  AFile_Socket& useSocket();
  
  const ATimer& getTimer() const;

  /*!
  AXmlEmittable
  */
  void emit(AXmlElement&) const;

  /*!
  HTTPS
  */
  bool isHttps() const;
  void setHttps(bool);

  /*!
  Read buffer
  */
  ARopeDeque& useBuffer() { return m_Buffer; }

private:
  AOSServices& m_Services;

  //a_ctor called by allocate()
  AOSRequest(AOSServices&, AFile_Socket *);

  //a_No copying
  AOSRequest(const AOSRequest& that) : m_Services(that.m_Services) {}
  AOSRequest& operator =(const AOSRequest&) { return *this; }

  AFile_Socket *mp_RequestFile;
  ATimer m_RequestTimer;

  bool m_isHttps;

  ARopeDeque m_Buffer;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSRequest_HPP__
