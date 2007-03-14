
#include "stdafx.h"

void showHelp();

int _tmain(int argc, _TCHAR* argv[])
{
  AMySQLServer server("AMud");
  if (!server.init())
  {
    std::cout << server.getError().c_str() << std::endl;
    return -1;
  }
  
  AObject oMOB("MOB"), oQuest("Quest");
  ANumber seq(0);
  AString strQuery, strLine, strCommand, strName;
  AFile_IOStream iostr(std::cout, std::cin);

  bool notdone = true;
  while(notdone)
  {
    std::cout << "\ndialogue:" << std::flush;
    iostr.readLine(strLine);
    strLine.getUntil(strCommand);
    if (!strCommand.compareNoCase("x"))
      notdone = false;
    else
    {
      if (!strCommand.compare("?"))
      {
        showHelp();
      }
      else if (!strCommand.compareNoCase("load"))
      {
        seq = 0;
        AString str;
        strLine.getUntil(str);
        strQuery = "SELECT * FROM `mob` WHERE id=";
        strQuery += str;
        if (server.executeSQL(strQuery, oMOB))
        {
          const AString& strName = oMOB.getValue("name");
          std::cout << "You have found " << strName << std::endl;
        }
        else
        {
          std::cout << server.getError().c_str() << std::endl;
          server.clearError();
        }
      }
      else if (!strCommand.compareNoCase("hail"))
      {
        seq = 0;
        if (oMOB.exists("id"))
        {
          strQuery = "SELECT * FROM `mob_dialogue` WHERE id=";
          strQuery += oMOB.getValue("dialogue_id");
          strQuery += " AND seq=";
          strQuery += seq.toString();
          strQuery += " ORDER BY seq";
          if (server.executeSQL(strQuery, oQuest))
          {
            std::cout << oMOB.getValue("name").c_str() << " says, \"" << oQuest.getValue("text") << "\"" << std::endl;
          }
          else
            std::cout << oMOB.getValue("name").c_str() << " says nothing..." << std::endl;
        }
      }
      else if (!strCommand.compareNoCase("tell"))
      {
        AString& strKeywords = oQuest.getValue("keywords");
        AString strWord;
        strLine.getUntil(strWord);
        if (!strKeywords.findNoCase(strWord))
        {
          ++seq;
          strQuery = "SELECT * FROM `mob_dialogue` WHERE id=";
          strQuery += oMOB.getValue("dialogue_id");
          strQuery += " AND seq=";
          strQuery += seq.toString();
          if (server.executeSQL(strQuery, oQuest))
          {
            std::cout << oMOB.getValue("name").c_str() << " says, \"" << oQuest.getValue("text") << "\"" << std::endl;
          }
          else
          {
            std::cout << oMOB.getValue("name").c_str() << " says nothing..." << std::endl;
            seq = 0;
          }
        }
        else
          std::cout << oMOB.getValue("name").c_str() << " only knows (" << strKeywords.c_str() << ")" << std::endl;
      }
      else if (!strCommand.compareNoCase("clear"))
      {
        oMOB.reset();
        oQuest.reset();
        server.clearError();
      }
      else if (!strCommand.compareNoCase("show"))
      {
        std::cout << oMOB.toXML().c_str() << std::endl;
        std::cout << oQuest.toXML().c_str() << std::endl;
      }
      else if (!strCommand.compareNoCase("error"))
      {
        std::cout << server.getError().c_str() << std::endl;
      }
      else
      {
        std::cout << "Command '" << strCommand.c_str() << "' not defined." << std::endl;
      }
    }
  }
  
  return 0;
}

void showHelp()
{
  std::cout << "MOB Transport" << std::endl;
  std::cout << "  load <object name>   - Load object from namespace" << std::endl;
  std::cout << "Actions" << std::endl;
  std::cout << "  hail                 - Initial hail" << std::endl;
  std::cout << "  tell [keyword]       - Ask about a keyword" << std::endl;
  std::cout << "Currently Loaded MOB Attribute Modification" << std::endl;
  std::cout << "  show                 - Show object's content" << std::endl;
  std::cout << "  clear                - Clear object's content" << std::endl;
  std::cout << "General" << std::endl;
  std::cout << "  error                - Displays any recent errors" << std::endl;
  std::cout << "  x                    - Exits" << std::endl;
}
