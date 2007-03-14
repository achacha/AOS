
#include "AObjectEngine.hpp"
#include "AMySQLServer.hpp"
#include "AFile_IOStream.hpp"
#include "AObject.hpp"


int main(int argc, char* argv[])
{
  AMySQLServer server("MySQL");
  AString strError;
  if (!server.init(strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }
  
  VECTOR_AString svTables, svFields;

  // Table list
  if (!server.getTables(svTables, strError))
  {
    std::cout << strError << std::endl;
    return -1;
  }

  std::cout << "Tables for " << server.toString().c_str() << std::endl;
  if (svTables.size() > 0)
  {
    for (size_t i=0; i<svTables.size(); ++i)
    {
      std::cout << "  " << svTables[i].c_str() << std::endl;
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
