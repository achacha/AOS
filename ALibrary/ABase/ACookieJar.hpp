/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ACookieJar_HPP__
#define INCLUDED__ACookieJar_HPP__

#include "apiABase.hpp"
#include "ACookie.hpp"
#include "templateListOfPtrs.hpp"
#include "templateMapOfPtrs.hpp"

class AHTTPRequestHeader;
class AHTTPResponseHeader;

/*!
Collection of cookies based on path
This is how a browser would store them, for server view use Cookies class instead
*/
class ABASE_API ACookieJar: public ADebugDumpable
{
public:
  /*!
  ctor
  */
  ACookieJar();

  /*!
  dtor
  */
  ~ACookieJar();

  /*!
  Add a new cookie to the jar
  Uses the path member of the cookie to correctly add it
  Domain must be set on the cookie
  
  @param pcookie Pointer to cookie that will be OWNED and DELETED by this class
  */
  void add(ACookie *pcookie);

  /*!
  Removes a cookie for a given domain
  
  @param domain of cookie
  @param path of cookie
  @param name of cookie
  @return true if removed, false if it didn't exist in the first place (in either case it is gone)
  */
  bool remove(const AString& domain, const AString& path, const AString& name);

  /*!
  Extract cookies in the response header via Set-Cookie: token
  Request is used to extract domain/path if not set by cookie

  @param request HTTP header
  @param response HTTP header
  */
  void parse(const AHTTPRequestHeader&, const AHTTPResponseHeader&);

  /*!
  Emit the appropriate cookies into a given request header that were added via the parse method
  Adds cookie pairs as Cookie: HTTP parameter that match the domain and path

  NOTE: Expired cookies will be deleted during this call so synchronize as needed

  @param header HTTP request
  */
  void emitCookies(AHTTPRequestHeader& header);

  /*!
  Emit cookies for a given path
  All cookies from a given path to root will be emitted

  Format is for an HTTP request header (e.g. foo=1; bar=2; ...)

  NOTE: Expired cookies will be deleted during this call so synchronize as needed

  @param path of the current request
  @param secureOnly if set only cookies flagged as secure will be emitted (for HTTPS usually)
  */
  void emitCookies(AOutputBuffer&, const AString& domain, const AString& path, bool secureOnly = false);

  /*!
  AEmittable
  Outputs domains contained only

  NOTE: Expired cookies will be deleted during this call so synchronize as needed
  */
  virtual void emit(AOutputBuffer&) const;
  
  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! No copy ctor or assign operator
  ACookieJar& operator=(const ACookieJar&) { return *this; }
  
  //! Path node that contains cookies and sub-path nodes (aka directory node)
  class Node : public ADebugDumpable
  {
  public:
    Node(Node *parent = NULL);

    /*!
    AEmittable
    Will walk up the parents until root and emit all along the way

    NOTE: Expired cookies will be deleted during this call so synchronize as needed
    
    @param target value of the Cookie: line (this is only the right hand side)
    @param if true only cookies marked SECURE will be emitted
    */
    void emitCookies(AOutputBuffer& target, bool secureOnly = false);

    /*!
    AEmittable
    Will walk up the parents until root and emit all along the way
    Will not expire any cookies
    */
    virtual void emit(AOutputBuffer&) const;

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  
    //! Collection of cookies at this node, name to ACookie *
    typedef AMapOfPtrs<AString, ACookie> COOKIES;
    COOKIES m_Cookies;

    //! Map of sub nodes
    typedef AMapOfPtrs<AString, Node> NODES;
    NODES m_Nodes;

    Node *mp_Parent;
  };

  //! Domain based cookie nodes
  typedef AMapOfPtrs<AString, Node> DOMAINS;
  DOMAINS m_Domains;

  //! Find or create a node for a given domain and path
  ACookieJar::Node *_getNode(const AString& domain, const AString& path, bool createIfNotFound = false);

  /*!
  If node not found, the highest existing node for a domain will be returned (root node at lowest if domain exists)
  Will return NULL if domain not found
  */
  ACookieJar::Node *_getNodeOrExistingParent(const AString& domain, const AString& path);
};

#endif
