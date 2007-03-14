#include <AString.hpp>
#include <ASQLSessionMSSQL.hpp>
#include <ASQLCommand.hpp>

int main()
{
  ASQLSessionMSSQL mssqlSession;
  if (mssqlSession.open("ACHACHA", "sa", ""))
  {
    ASQLCommand sql;
    sql.setCommand("SELECT * FROM Repository..NameValuePairs WHERE Name LIKE 'case.%' ");
    mssql.execute(sql);
  }

  return 0x0;
}
