/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"

const AString AResultSet::ROW("row");

void AResultSet::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_SQL=" << m_SQL << std::endl;

  //a_Results
  ADebugDumpable::indent(os, indent+1) << "m_ResultSet={" << std::endl;
  RESULTSET::const_iterator citRow = m_ResultSet.begin();
  while(citRow != m_ResultSet.end())
  {
    ADebugDumpable::indent(os, indent+2) << "{" << std::endl;
    for (size_t u=0; u < (*citRow).size(); ++u)
    {
      //a_first is field DB name, second is the value in that field
      ADebugDumpable::indent(os, indent+3) << '{' << m_FieldNames.at(u) << '=' << (*citRow).at(u) << '}' << std::endl;
    }
    ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    ++citRow;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AResultSet::emit(AOutputBuffer& target) const
{
  target.append("{sql:", 5);
  target.append(m_SQL);
  target.append('}');
  target.append(AConstant::ASTRING_CRLF);

  RESULTSET::const_iterator citRow = m_ResultSet.begin();
  while(citRow != m_ResultSet.end())
  {
    if ((*citRow).size() != m_FieldNames.size())
      ATHROW(this, AException::InvalidObject);

    target.append('{');
    for (size_t u=0; u < (*citRow).size(); ++u)
    {
      //a_first is field DB name, second is the value in that field
      target.append('{');
      target.append(m_FieldNames.at(u));
      target.append(':');
      target.append((*citRow).at(u));
      target.append('}');
    }
    target.append('}');
    target.append(AConstant::ASTRING_CRLF);
    ++citRow;
  }
}

AXmlElement& AResultSet::emitXml(AXmlElement& thisRoot) const
{
  AASSERT(this, !thisRoot.useName().isEmpty());

  thisRoot.addElement(ASW("sql",3)).addData(m_SQL, AXmlElement::ENC_CDATADIRECT);

  RESULTSET::const_iterator citRow = m_ResultSet.begin();
  while(citRow != m_ResultSet.end())
  {
    if ((*citRow).size() != m_FieldNames.size())
      ATHROW(this, AException::InvalidObject);

    AXmlElement& elementRow = thisRoot.addElement(ROW);
    for (size_t u=0; u < (*citRow).size(); ++u)
    {
      //a_DB column name and data in that column for given row
      elementRow.addElement(m_FieldNames.at(u)).addData((*citRow).at(u));
    }
    ++citRow;
  }

  return thisRoot;
}

AResultSet::AResultSet()
{
}

AResultSet::AResultSet(const AResultSet& that) :
  m_ResultSet(that.m_ResultSet)
{

}

AResultSet::~AResultSet()
{
}

void AResultSet::clear()
{
  m_FieldNames.clear();
  m_ResultSet.clear();
}

VECTOR_AString& AResultSet::insertRow()
{
  AASSERT(this, m_ResultSet.size() < DEBUG_MAXSIZE_AResultSet);
  m_ResultSet.push_back(VECTOR_AString());
  return m_ResultSet.back();
}

VECTOR_AString& AResultSet::useFieldNames()
{ 
  return m_FieldNames;
}

const VECTOR_AString& AResultSet::getFieldNames() const
{ 
  return m_FieldNames;
}

const AString& AResultSet::getFieldName(size_t column)
{
  if (column >= m_FieldNames.size())
    ATHROW(this, AException::InvalidParameter);

  return m_FieldNames.at(column);
}

size_t AResultSet::getRowCount() const
{
  return m_ResultSet.size();
}

const VECTOR_AString& AResultSet::getRow(size_t row) const
{
  if (row >= m_ResultSet.size())
    ATHROW(this, AException::InvalidParameter);

  return m_ResultSet.at(row);
}

const AString& AResultSet::getData(size_t nRow, size_t nColumn)
{
  if (nRow >= m_ResultSet.size())
    ATHROW(this, AException::InvalidParameter);

  VECTOR_AString& row = m_ResultSet.at(nRow);
  if (nColumn >= row.size())
    ATHROW(this, AException::InvalidParameter);

  return row.at(nColumn);
}

size_t AResultSet::getFieldIndex(const AString& fieldName)
{
  size_t ret = AConstant::npos;
  for (size_t u=0; m_FieldNames.size(); ++u)
    if (m_FieldNames.at(u).equals(fieldName))
      return u;

  if (AConstant::npos == ret)
    ATHROW(this, AException::DoesNotExist);

  return ret;
}

size_t AResultSet::getFieldCount() const
{
  return m_FieldNames.size();
}



AString& AResultSet::useSQL()
{
  return m_SQL;
}