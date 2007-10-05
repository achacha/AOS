#ifndef INCLUDED__AResultSet_HPP__
#define INCLUDED__AResultSet_HPP__

#include "apiABase.hpp"
#include "AObjectBase.hpp"

class ABASE_API AResultSet : public AObjectBase
{
public:
  /*!
  name - name of the current object (AObjectContainer can override this if it is explicitly specified during insertion)
  */
  AResultSet();
  AResultSet(const AResultSet&);
  virtual ~AResultSet();
  
  /*!
  SQL query access
  Set by DB when result set is generated
  */
  AString& useSQL();

  /*!
  Creates a new row of data and returns referece to it
  */
  VECTOR_AString& insertRow();
  
  /*!
  Access to result data via 0 based index
  [0, rowCount), [0, fieldCount)  (first row or field is indexed at 0 unlike databases)
  */
  const VECTOR_AString& getRow(size_t rowIndex) const;
  const AString& getData(size_t rowIndex, size_t fieldIndex);

  /*!
  Access to result set properties
  The result set is a grid of row and field
  */
  size_t getRowCount() const;
  size_t getFieldCount() const;                  //a_aka. Column count
  VECTOR_AString& useFieldNames();
  const VECTOR_AString& getFieldNames() const;
  const AString& getFieldName(size_t fieldIndex);
  size_t getFieldIndex(const AString& fieldName);

  /*!
  Clearing the result set
  */
  void clear();

  /*!
  AXmlEmittable and AEmittable call the subpath method with empty path
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emitXml(AXmlElement&) const;
  
  /*!
  Clone self
  */
  AObjectBase* clone() const { return new AResultSet(*this); }  //a_Clone self using copy ctor (this can just be copied as is)

private:
  
  //a_Query that generated this result, usually set by the database and here for reference
  AString m_SQL;

  //a_Field names
  VECTOR_AString m_FieldNames;

  //a_Rows
  typedef std::vector<VECTOR_AString> RESULTSET;
  RESULTSET m_ResultSet;

  static const AString ROW;      //a_Name of 'row' for emits

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AResultSet_HPP__
