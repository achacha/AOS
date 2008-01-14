#ifndef INCLUDED__ADatabase_HPP__
#define INCLUDED__ADatabase_HPP__

#include "apiABase.hpp"
#include "AString.hpp"
#include "ADebugDumpable.hpp"
#include "AXmlEmittable.hpp"
#include "AUrl.hpp"

class AResultSet;

/*!
Interface for a database that can support loading and saving of AObjectNamespace

Required to implement by derived class:
------------CUT BELOW----------
public:
  virtual bool init(AString& error);
  virtual bool reconnect();
  virtual void deinit(AString& error);
  virtual u4 executeSQL(const AString& query, AResultSet&, AString& error);
  virtual emitXml(AXmlElement& target) const;
  virtual ADatabase *clone() const;

//OPTIONAL  
  virtual bool isInitialized() const;
------------CUT ABOVE----------
*/
class ABASE_API ADatabase : public AXmlEmittable, public ADebugDumpable
{
public:
  ADatabase();
  ADatabase(const AUrl& urlConnection);
  virtual ~ADatabase();

  /*!
  AXmlElement
  */
  void emitXml(AXmlElement& target) const;

  /*!
  Initialization and deinitialization
  */
  virtual bool init(AString& error) = 0;
  virtual bool isInitialized() const;
  virtual void deinit() = 0;

  /*!
  exeuteSQL will call this if it detects an issue with a lost connection
  */
  virtual bool reconnect(AString& error) = 0;

  /*!
  Executes a query and inserts an object into the targetNS with resultSetName
  
  Populates the resultSet container as follows:
  <ResultSet sql="SELECT * from user" rows=2>
    <row n='1'>
      <id>1298</id>
      <username>foo</username>
    </row>
    <row n='2'>
      <id>2123</id>
      <username>bar</username>
    </row>
  </ResultSet>
  
  If the resultSetName is "/query/MyQueryResult", then a new object called MyQueryResult is created in
  path /query and a container with above results is added

  Returns # of rows read, if return == AConstant::npos, then error occured and 'error' string was set
  */
  virtual size_t executeSQL(const AString& query, AResultSet&, AString& error) = 0;

  /*!
  Factory method for cloning existing connection
  Creates a new ADatabase and calls init()
  If NULL is returned, error will contain the reason
  */
  virtual ADatabase *clone(AString& error) const = 0;

  /*!
  Connection URL
  */
  const AUrl& getUrl() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  AUrl m_urlConnection;
  bool mbool_Initialized;
};

#endif //INCLUDED__ADatabase_HPP__
