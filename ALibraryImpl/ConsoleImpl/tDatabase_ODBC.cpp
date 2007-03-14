#include "AString.hpp"
#include "AResultSet.hpp"
#include "AXmlElement.hpp"
#include "ARope.hpp"
#include "ABitArray.hpp"

#include "AODBCServer.hpp"
#include "AMySQLServer.hpp"

void testSelect(ADatabase& db)
{
  AString error;
  AResultSet rs;
  
  u4 rows = db.executeSQL("SELECT * from global", rs, error);
  if (AString::npos == rows)
  {
    std::cerr << error << std::endl;
  }
  else
  {
    std::cout << "Selected " << rows << " rows." << std::endl;
    
    AXmlElement root("root");
    rs.emit(root);
    ARope rope;
    root.emitXml(rope, 0);
    std::cout << rope << std::endl;
  }

  u4 myNameCol = rs.getFieldIndex("name");
  u4 myValueCol = rs.getFieldIndex("value");
  for (u4 row=0; row<rs.getRowCount(); ++row)
  {
    const AString& name = rs.getData(row, myNameCol);
    if (name.equals("test_query"))
    {
      AQueryString qs(rs.getData(row, myValueCol));
      AString str;
      qs.emit(str);
      std::cout << "Query string detected: " << str << std::endl;
    }
    else if (name.equals("flags"))
    {
      ABitArray flags(rs.getData(row, myValueCol));
      AString str;
      flags.emit(str);
      std::cout << "Flags found: " << str << std::endl;

      str.clear();
      flags.setOutputMode(ABitArray::Binary);
      flags.emit(str);
      std::cout << "Flags found: " << str << std::endl;
    }
  }
}

void testInsert(ADatabase& db)
{
  AString error;
  AResultSet rs;

  u4 rows = db.executeSQL("INSERT INTO global (name,value) VALUES ('inserted', 'foo')", rs, error);
  if (AString::npos == rows)
  {
    std::cerr << error << std::endl;
  }
  else
  {
    std::cout << "Inserted " << rows << " rows successfully." << std::endl;
  }
}

void testUpdate(ADatabase& db)
{
  AString error;
  AResultSet rs;
  
  u4 rows = db.executeSQL("UPDATE global SET value='bar' WHERE name='inserted'", rs, error);
  if (AString::npos == rows)
  {
    std::cerr << error << std::endl;
  }
  else
  {
    std::cout << "Updated " << rows << " rows successfully." << std::endl;
  }
}

void testDelete(ADatabase& db)
{
  AString error;
  AResultSet rs;

  u4 rows = db.executeSQL("DELETE FROM global WHERE name='inserted'", rs, error);
  if (AString::npos == rows)
  {
    std::cerr << error << std::endl;
  }
  else
  {
    std::cout << "Deleted " << rows << " rows successfully." << std::endl;
  }
}

int main()
{
  AString error;
  
  AUrl url("odbc://aos:aos@localhost/aos_global/");
  AODBCServer db(url);
  
//a_AMySQLServer equivalent
//  AUrl url("mysql://aos:aos@localhost/aos/");
//  AMySQLServer db(url);

  if (!db.init(error))
  {
    db.debugDump();
    std::cerr << error << std::endl;
    return -1;
  }

  testInsert(db);
  testUpdate(db);
  testSelect(db);
  testDelete(db);

  return 0;
}
