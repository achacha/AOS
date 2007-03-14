#include <AFilePhysical.hpp>
#include <ARandomNumberGenerator.hpp>

int testRandom(int iPeek, int iRead, bool boolReadOnly = false)
{
  AFilePhysical file;
  if (file.open("C:/temp/sample.txt"))
  {
    AString str(1024, 128);
    int iR;
    u4 uRead = -1;
    do
    {
      if ( ! boolReadOnly)
      {
        iR = ARandomNumberGenerator::intRange(iPeek, 1);
        uRead = file.peek(str.getCharPtr(), iR);
        str[uRead] = '\x0';
        cout << "peek[" << iR << "," << uRead << "]=" << str.c_str() << endl;

        if (!uRead)
           break;
      }

      iR = ARandomNumberGenerator::intRange(iRead, 1);
      uRead = file.read(str.getCharPtr(), iR);
      str[uRead] = '\x0';
      cout << "read[" << iR << "," << uRead << "]=" << str.c_str() << endl;
    } while( uRead );
   
    file.close();
  }
  else
    cout << "Could not open file." << endl;

  return 0x0;
}

int testFixed(int iPeek, int iRead)
{
  AFilePhysical file;
  if (file.open("C:/temp/sample.txt"))
  {
    AString str(1024, 128);
    u4 uRead = -1;
    do
    {
      uRead = file.peek(str.getCharPtr(), iPeek);
      str[uRead] = '\x0';
      cout << "peek[" << uRead << "]=" << str.c_str() << endl;

      if (!uRead)
         break;

      uRead = file.read(str.getCharPtr(), iRead);
      str[uRead] = '\x0';
      cout << "read[" << uRead << "]=" << str.c_str() << endl;
    } while( uRead );
   
    file.close();
  }
  else
    cout << "Could not open file." << endl;

  return 0x0;
}

void testLineRead()
{
  AFilePhysical file;
  if (file.open("C:/temp/sample.txt"))
  {
    AString str;
    u4 u4Read = -1;
    do
    {
      u4Read = file.readLine(str);
      cout << "Length=" << u4Read << "  Content=" << str.c_str() << endl;
    }
    while (u4Read);
  }
}

void showHelp()
{
  cerr << "AFilePhysical Lab\r\n";
  cerr << "Random Peek/Read: 1 <peek> <read>" << endl;
  cerr << "Random Read     : 2 <peek> <read>" << endl;
  cerr << "Fixed  Peek/Read: 3 <peek> <read>" << endl;
  cerr << "Line read       : 4" << endl;
}

int main(int iArgCount, char **ppcArgValues)
{
  if (iArgCount < 2)
  {
    showHelp();
    return -1;
  }

  int i1, i2;

  switch(*ppcArgValues[1])
  {
    case '1' :
      if (iArgCount < 4) { showHelp(); return -1; }

      i1 = atoi(ppcArgValues[2]);
      i2 = atoi(ppcArgValues[3]);
      cout << "peek=" << i1 << "  read=" << i2 << endl;

      cout << "Random peek(" << i1 << ") and read(" << i2 << ")" << endl;
      testRandom(i1, i2);
    break;

    case '2' :
      if (iArgCount < 4) { showHelp(); return -1; }

      i1 = atoi(ppcArgValues[2]);
      i2 = atoi(ppcArgValues[3]);
      cout << "peek=" << i1 << "  read=" << i2 << endl;

      cout << "Random read(" << i2 << ")" << endl;
      testRandom(i1, i2, true);
    break;
    
    case '3' :
      if (iArgCount < 4) { showHelp(); return -1; }

      i1 = atoi(ppcArgValues[2]);
      i2 = atoi(ppcArgValues[3]);
      cout << "peek=" << i1 << "  read=" << i2 << endl;

      cout << "Fixed peek(" << i1 << ") and read(" << i2 << ")" << endl;
      testFixed(i1, i2);
    break;

    case '4' :
      testLineRead();
    break;
    
    default:
      showHelp();
  }

  return 0x0;
}