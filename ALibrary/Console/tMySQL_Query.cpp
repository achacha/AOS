
#include "AMySQLServer.hpp"
#include "AFile_IOStream.hpp"
#include "AObjectContainer.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: this mysql://<username>:<password>@[hostname]/[database]/ <query>" << std::endl;
    std::cout << "If <query> is omitted, database metadata is dumped, trailing / required in url." << std::endl;
    return -1;
  }

  AString str, strError;
  AUrl url(argv[1]);

  AMySQLServer server(url);
  if (!server.init(strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }

  if (argc == 2)
  {
    //a_Metadata dump only
    VECTOR_AString svTables, svFields;

    // Table list
    if (!server.getTables(svTables, strError))
    {
      std::cout << strError << std::endl;
      return -1;
    }

    server.emit(str);
    std::cout << "Tables for " << str << std::endl;
    if (svTables.size() > 0)
    {
      for (size_t i=0; i<svTables.size(); ++i)
      {
        std::cout << "  " << svTables[i] << std::endl;
        if (!server.getFields(svTables[i], svFields, strError))
        {
          std::cout << strError << std::endl;
          return -1;
        }
        if (svFields.size() > 0)
        {
          for (size_t j=0; j<svFields.size(); ++j)
            std::cout << "    " << svFields[j].c_str() << std::endl;
          std::cout << std::endl;
        }
        else
          std::cout << "    No fields found." << std::endl;
      }
      std::cout << std::endl;
    }
    else
      std::cout << "  No tables found." << std::endl;
    
    return 0;
  }
  
  AString strQuery;
  AObjectContainer ns("dbquery");
  for (int i=2; i<argc; ++i)
  {
    strQuery.append(argv[i]);
    strQuery.append(AString::sstr_Space);
  }
  strQuery.stripTrailing();
  
  u4 rows = server.executeSQL(strQuery, ns, strError);
  if (AString::npos == rows)
  {
    std::cout << strError << std::endl;
    return -1;
  }

/*
//a_Get field and row info
  MYSQL_FIELD *fields = mysql_fetch_fields(pmyresult);
  int field_count = mysql_num_fields( pmyresult );
  int row_count = (int)mysql_num_rows(pmyresult);
  int current_row = 0;

  //a_Loop
  AString strOut, str;
  bool ret = true;
  MYSQL_ROW myrow;
  strOut.append("<dbquery database=\"");
  strOut.append(argv[1]);
  strOut.append("\" query=\"");
  strOut.append(strQuery);
  strOut.append("\">");
  strOut.append(AString::sstr_CRLF);
  while (current_row < row_count)
  {
    myrow = mysql_fetch_row( pmyresult );
    if (!myrow)
      break;

    strOut.append("  <dbrow id=");
    strOut.append(AString::fromInt(current_row));
    strOut.append(">");
    strOut.append(AString::sstr_CRLF);
    for (int i=0; i < field_count; ++i)
    {
      ret &= AMySQLServer::convert(myrow, i, str);
      strOut.append("    <");
      strOut.append(fields[i].name);
      if (str.length() > 0)
      {
        strOut.append(">");
        strOut.append(str);
        strOut.append("</");
        strOut.append(fields[i].name);
        strOut.append(">");
      }
      else
      {
        strOut.append("/>");
      }
      strOut.append(AString::sstr_CRLF);
    }
    strOut.append("  </dbrow>");
    strOut.append(AString::sstr_CRLF);

    ++current_row;
  }
  strOut.append("</dbquery>");
  std::cout << strOut << std::endl;
*/

  str.clear();
  ns.emit(str, 0);
  std::cout << str << std::endl;

  str.clear();
  ns.emitXml(str, 0);
  std::cout << str << std::endl;

  return 0;
}
