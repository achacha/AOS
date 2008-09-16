/*
Written by Alex Chachanashvili

$Id$
*/
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
  //! ctor
  ADatabase();
  
  //! ctor from AUrl
  ADatabase(const AUrl& urlConnection);
  
  //! virtual dtor
  virtual ~ADatabase();

  /*!
  AXmlElement

  @param thisRoot to append XML elements to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Initialization

  @param error to append if any occured
  @return true if an error occured
  */
  virtual bool init(AString& error) = 0;

  /*!
  Check if initialized
  */
  virtual bool isInitialized() const;

  /*!
  Initialization and deinitialization
  */
  virtual void deinit() = 0;

  /*!
  exeuteSQL will call this if it detects an issue with a lost connection

  @param error to append if any occured
  @return true if an error occured
  */
  virtual bool reconnect(AString& error) = 0;

  /*!
  Executes a query and inserts an object into the targetNS with resultSetName
  
  Populates the resultSet container as follows:
\xmlonly
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
\endxmlonly

  If the resultSetName is "/query/MyQueryResult", then a new object called MyQueryResult is created in path /query and a container with above results is added

  @param query of SQL to execute
  @param target result set
  @param error to append error to if any
  @return number of rows read, if return == AConstant::npos, then error occured and 'error' string was set
  */
  virtual size_t executeSQL(const AString& query, AResultSet& target, AString& error) = 0;

  /*!
  Connection URL

  @return constant reference to URL used to connect this database
  */
  const AUrl& getUrl() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

   /*!
  Factory method for cloning existing connection
  Creates a new ADatabase and calls init()
  If NULL is returned, error will contain the reason
  
  @param error appended if not cloned
  @return cloned object
  */
  virtual ADatabase *clone(AString& error) const = 0;

protected:
  AUrl m_urlConnection;
  bool mbool_Initialized;
};

#endif //INCLUDED__ADatabase_HPP__
