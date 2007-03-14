#include <AString.hpp>

#define DBNTWIN32
#include <windows.h>
#include <SQLFront.h>
#include <SQLDB.h>

#pragma comment(lib, "NTWDBLIB.LIB")

int err_handler(PDBPROCESS, INT, INT, INT, LPCSTR, LPCSTR);
int msg_handler(PDBPROCESS, DBINT, INT, INT, LPCSTR, LPCSTR, LPCSTR, DBUSMALLINT);

int main()
{
  dberrhandle(err_handler);
  dbmsghandle(msg_handler);

  dbinit();

  PLOGINREC pLogin = dblogin();
  if (!pLogin)
  {
    cerr << "Cannot allocate login structure" << endl;
    return 0x1;
  }

  if (SUCCEED != DBSETLUSER(pLogin, "sa"))
  {
    cerr << "Unable to set username" << endl;
    return 0x2;
  }

  if (SUCCEED != DBSETLPWD(pLogin, ""))
  {
    cerr << "Unable to set password" << endl;
    return 0x3;
  }
  
  PDBPROCESS pdbProcess = dbopen(pLogin, "achacha");
  if (!pdbProcess)
  {
    cerr << "Unable to open database" << endl;
    return 0x4;
  }

  // First, put the command into the command buffer. 
  dbcmd (pdbProcess, "SELECT * FROM Repository..NameValuePairs WHERE Name LIKE 'case.%' ");

  // Send the command to SQL Server and start execution. 
  dbsqlexec (pdbProcess);    // Process the results. 
  if (dbresults (pdbProcess) == SUCCEED)
  {
    unsigned char dbstrName[256], dbstrValue[4096];
    
    // Bind column to program variables. 
    dbbind(pdbProcess, 1, NTBSTRINGBIND, 0, dbstrName);
    dbbind(pdbProcess, 2, NTBSTRINGBIND, 0, dbstrValue);
  
    // Retrieve and print the result rows. 
    while (dbnextrow (pdbProcess) != NO_MORE_ROWS)
      cout << dbstrName << "=" << dbstrValue << endl;
  }

  //a_Exit
  if (SUCCEED != dbclose(pdbProcess))
  if (!pdbProcess)
  {
    cerr << "Unable to close database" << endl;
    return 0x4;
  }
  dbexit();
  
  return 0x0;
}

int err_handler(PDBPROCESS dbproc,
                INT severity,
                INT dberr,
                INT oserr,
                LPCSTR dberrstr,
                LPCSTR oserrstr)
{
  cerr << "DB-Library Error " << dberr << ": " << dberrstr << endl;

  if (oserr != DBNOERR)
    cerr << "Operating System Error " << oserr << ": " << oserrstr << endl;

  return INT_CANCEL;
}

int msg_handler(PDBPROCESS dbproc,
                DBINT msgno,
                INT msgstate,
                INT severity,
                LPCSTR msgtext,
                LPCSTR server,
                LPCSTR procedure,
                DBUSMALLINT line)
{
    cerr << "SQL Server Message " << msgno << ": " << msgtext << endl;
    
    return 0x0;
}
