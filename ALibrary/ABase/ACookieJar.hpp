#ifndef INCLUDED__ACookieJar_HPP__
#define INCLUDED__ACookieJar_HPP__

#include "apiABase.hpp"
#include "ACookie.hpp"
#include "templateListOfPtrs.hpp"
#include "templateMapOfPtrs.hpp"

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
  
  @param header HTTP response
  */
  void parse(const AHTTPResponseHeader& header);

  /*!
  Emit cookies for a given path
  All cookies from a given path to root will be emitted

  Format is for an HTTP request header (e.g. foo=1; bar=2; ...)

  @param path of the current request
  @param secureOnly if set only cookies flagged as secure will be emitted (for HTTPS usually)
  */
  void emit(AOutputBuffer&, const AString& domain, const AString& path, bool secureOnly = false) const;

  /*!
  AEmittable
  Outputs domains contained only
  */
  virtual void emit(AOutputBuffer&) const;
  
  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  class Node : public ADebugDumpable
  {
  public:
    Node(Node *parent = NULL);

    /*!
    Emit secure only cookie
    Will walk up the parents until root and emit all secure along the way
    */
    void emitSecureOnly(AOutputBuffer&) const;

    /*!
    AEmittable
    Will walk up the parents until root and emit all along the way
    */
    virtual void emit(AOutputBuffer&) const;

    /*!
    ADebugDumpable
    */
    virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  
    //! Collection of cookies at this node, name to ACookie*
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
  const ACookieJar::Node *_getNodeOrExistingParent(const AString& domain, const AString& path) const;
};

#endif
