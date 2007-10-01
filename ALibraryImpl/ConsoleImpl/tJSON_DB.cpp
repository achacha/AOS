#include "AXmlDocument.hpp"
#include "AResultSet.hpp"
#include "AMySQLServer.hpp"
#include "AFile_IOStream.hpp"

int main()
{
  AUrl dbUrl(ASWNL("mysql://aos:aos@equinox/aos/"));
  AMySQLServer db(dbUrl);
  AString str;
  if (db.init(str))
  {
    AResultSet rs;
    db.executeSQL(ASWNL("select * from user"), rs, str);
    if (!str.isEmpty())
      std::cout << str << std::endl;
    
    AXmlDocument doc(ASWNL("root"));
    rs.emit(doc.useRoot());

//    rs.debugDump();
//    doc.debugDump();
//    return 0;

    AFile_IOStream io;
    doc.emit(io, 0);
    AConstant::ASTRING_CRLFCRLF.emit(io);
    doc.emitJSON(io, 0);
    //doc.emitJSON(io);

  }
  else
    std::cout << str << std::endl;

  db.deinit();
  return 0;
}