#include <stdio.h>
#include <AString.hpp>
#include <ATextConverter.hpp>
#include <AFilePhysical.hpp>

//
// Any input that is \uXXXX will be converted to UTF-8 all else will be ignored
//
void convertToUTF8_thisSUX()
{
  AString strWork, strRune, strResult;
  const int iiBufferSize = 0x10;
  char sBuffer[iiBufferSize + 0x1];
  wchar_t wc;
  int iPos = 0x0;

  //a_This cannot read broken lines yet!
  while (!feof(stdin))
  {
    sBuffer[0x0] = '\x0';
    fread(sBuffer, sizeof(char), iiBufferSize, stdin);
    sBuffer[iiBufferSize] = '\x0';
    strWork.append(sBuffer);
    iPos = 0x0;

    while (strWork.length() >= 0x6)
    {
      while ((iPos = strWork.find("\\u")) != AString::npos)
      {
        //a_Not enough read more
        if (strWork.length() < 0x6)
          break;

        strWork.peek(strRune, iPos + 0x2, 0x4);
        wc = strRune.toLong(0x10);
        strResult += AString::fromUnicode(wc);
        strWork.remove(iPos + 0x6);
        iPos = 0x0;
      }

      if (iPos == AString::npos)
        break;
    }
  }

  cout << strResult.c_str() << endl;
}

void convertToUTF8__(const AString& strInFilename, const AString& strOutFilename)
{
  AFilePhysical fileIn;
  if (fileIn.open(strInFilename))
  {
     AFilePhysical fileOut;
     if (fileOut.open(strOutFilename, AFilePhysical::smstr_Write))
     {
        AString strRead;
        u1 uX;
        const AString cstrDelim("\\");
        AString strData(0x5, 0x4);
        wchar_t wc;

        while (1)
        {
          if (fileIn.readUntil(strRead, cstrDelim) == 0x0)
            break;
          fileOut.write(strRead.c_str(), strRead.length());

          fileIn.read(uX);
          switch(uX)
          {
            case 'u' : 
              fileIn.read(strData.startUsingCharPtr(), 0x4);
              strData.startUsingCharPtr()[0x4] = '\x0';
              strData.stopUsingCharPtr();
              wc = strData.toLong(0x10);
              strRead = AString::fromUnicode(wc);
              fileOut.write(strRead.c_str(), strRead.length());
            break;

            default:
              fileOut.write(uX);
          }
        }
     }
     else
       cerr << "Unable to open " << strOutFilename.c_str() << "for write." << endl;
  }
  else
    cerr << "Unable to open " << strInFilename.c_str() << " for read." << endl;
}

void convertToUTF8(const AString& strInFilename, const AString& strOutFilename)
{
  AFilePhysical fileIn;
  if (fileIn.open(strInFilename))
  {
     u4 uLength = fileIn.getLength();
     AString strInput(uLength, 0x10);
     fileIn.read(strInput.startUsingCharPtr(), uLength);
     fileIn.close();

     AFilePhysical fileOut;
     if (fileOut.open(strOutFilename, AFilePhysical::smstr_Write))
     {
       AString strOutput;
       AString strRune;
       int iPos = 0x0;
       wchar_t wc;
       while (iPos < uLength)
       {
         switch(strInput.at(iPos))
         {
           case '\\' :
             iPos++;
             if ((iPos + 0x4) < uLength)
             {
               if (strInput.at(iPos) == 'u')
               {
                 //a_Decode
                 iPos++;
                 strInput.peek(strRune, iPos, 0x4);
                 iPos += 0x4;
                 wc = strRune.toLong(0x10);
                 strOutput += AString::fromUnicode(wc);
               }
               else if (strInput.at(iPos) == '\\')
               {
                 strOutput += strInput.at(iPos++);
               }

             }
             else
             {
               strOutput += '\\';
               strOutput += strInput.at(iPos++);
             }
           break;

           default :
             strOutput += strInput.at(iPos++);

         }
       }
       
       fileOut.write(strOutput.data(), strOutput.length());
       fileOut.close();
     }
     else
       cerr << "Unable to open " << strOutFilename.c_str() << "for write." << endl;
  }
  else
    cerr << "Unable to open " << strInFilename.c_str() << " for read." << endl;
}

int main(int iArgCount, char** ppcArgValues)
{
  if (iArgCount < 0x3)
  {
    cout << "Usage: program [input filename] [output filename]" << endl;
    return -1;
  }
  
  convertToUTF8(ppcArgValues[0x1], ppcArgValues[0x2]);

  return 0x0;
}
