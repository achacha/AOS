// ObjectEngineTest.cpp : Defines the entry point for the console application.
//

#include <AFile_IOStream.hpp>
#include <ANumber.hpp>
#include <AObjectEngine.hpp>
#include <AMySQLServer.hpp>
#include <AObject.hpp>

void showHelp();

int main(int argc, char* argv[])
{
  AMySQLServer server("AObjects");
  if (!server.init())
  {
    std::cout << server.getError().c_str() << std::endl;
    return -1;
  }
  AObjectEngine engine(server);
  
  AObject object;

  AString strLine, strCommand, strName;
  AFile_IOStream iostr;

  bool notdone = true;
  while(notdone)
  {
    std::cout << "object>" << std::flush;
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
      else if (!strCommand.compareNoCase("list"))
      {
        if (strLine.isEmpty())
          strName = object.getNamespace();
        else
          strName = strLine;

        if (strName.isEmpty())
          std::cout << "Must specify a namespace." << std::endl;
        else
        {
          VECTOR_AString svObjects;
          if (engine.listObjects(strName, svObjects))
          {
            size_t iSize = svObjects.size();
            for (size_t i=0; i<iSize; ++i)
              std::cout << "'" << svObjects.at(i).c_str() << "'\t";
            std::cout << std::endl;
          }
          else
          {
            std::cout << "ERROR: " << engine.getError().c_str() << std::endl;
            engine.clearError();
          }
        }
      }
      else if (!strCommand.compareNoCase("load"))
      {
        object.parse(strLine);
        
        if (engine.loadObject(object))
          std::cout << "Object named '" << object.getName().c_str() << "' loaded." << std::endl;
        else
        {
          std::cout << "ERROR: " << engine.getError().c_str() << std::endl;
          engine.clearError();
        }
      }
      else if (!strCommand.compareNoCase("save"))
      {
        object.parse(strLine);

        if (engine.saveObject(object))
          std::cout << "Object named '" << object.getName().c_str() << "' saved." << std::endl;
        else
        {
          std::cout << "ERROR: " << engine.getError() << std::endl;
          engine.clearError();
        }
      }
      else if (!strCommand.compareNoCase("erase"))
      {
        strName = strLine;
        if (!strName.isEmpty())
        {
          object.setName(strName);
        }
        if (object.getName().isEmpty())
          std::cout << "Must specify a name for the object" << std::endl;
        else
        {
          if (engine.eraseObject(object))
            std::cout << "Object named '" << object.getName().c_str() << "' erased." << std::endl;
          else
          {
            std::cout << "ERROR: " << engine.getError() << std::endl;
            engine.clearError();
          }
        }
      }
      else if (!strCommand.compareNoCase("rename"))
      {
        strName = strLine;
        if (object.getName().isEmpty())
          std::cout << "Must have a named object to rename" << std::endl;
        else if (strName.isEmpty())
          std::cout << "Must specify a new name for the current object" << std::endl;
        else
        {
          if (engine.renameObject(object, strName))
            std::cout << "Object named '" << object.getName().c_str() << "' renamed '" << strName.c_str() << "'." << std::endl;
          else
          {
            std::cout << "ERROR: " << engine.getError() << std::endl;
            engine.clearError();
          }
        }
      }
      else if (!strCommand.compareNoCase("set"))
      {
        object.parse(strLine);
      }
      else if (!strCommand.compareNoCase("show"))
      {
        if (!object.getName().isEmpty())
        {
          std::cout << object.toString().c_str() << std::endl;
        }
        else
          std::cout << "Object has not been named yet";
      }
      else if (!strCommand.compareNoCase("namespaces"))
      {
        VECTOR_AString svNamespaces;
        if (engine.listNamespaces(svNamespaces))
        {
          size_t iSize = svNamespaces.size();
          for (size_t i=0; i<iSize; ++i)
            std::cout << "'" << svNamespaces.at(i).c_str() << "'\t";
          std::cout << std::endl;
        }
        else
        {
          std::cout << "ERROR: " << engine.getError().c_str() << std::endl;
          engine.clearError();
        }
      }
      else if (!strCommand.compareNoCase("use"))
      {
        strName = strLine;
        if (strName.isEmpty())
          std::cout << "Must provide a valid name for the namespace" << std::endl;
        else
        {
          object.setNamespace(strName);
          std::cout << "Active namespace is '" << strName.c_str() << "'" << std::endl;
        }
      }
      else if (!strCommand.compareNoCase("create"))
      {
        strName = strLine;
        if (strName.isEmpty())
          std::cout << "Must provide a valid name for the namespace" << std::endl;
        else
        {
          engine.createNamespace(strName);
          std::cout << "Created namespace '" << strName.c_str() << "'" << std::endl;
        }
      }
      else if (!strCommand.compareNoCase("destroy"))
      {
        strName = strLine;
        if (strName.isEmpty())
          std::cout << "Must provide a valid name for the namespace" << std::endl;
        else
        {
          engine.destroyNamespace(strName);
          std::cout << "Destroyed namespace '" << strName.c_str() << "'" << std::endl;
        }
      }
      else if (!strCommand.compareNoCase("object"))
      {
        if (strLine.find(' ') != AString::npos)
        {
          std::cout << "Spaces are not allowed in object names." << std::endl;
        }
        else
        {
          strName = strLine;
          if (strName.isEmpty())
            std::cout << "Must provide a valid name for the object" << std::endl;
          else
          {
            object.setName(strName);
            std::cout << "Object is now called '" << strName.c_str() << "'" << std::endl;
          }
        }
      }
      else if (!strCommand.compareNoCase("clear"))
      {
        object.reset();
      }
      else if (!strCommand.compareNoCase("status"))
      {
        std::cout << "Current status" << std::endl;
        std::cout << "  Database : " << engine.toString().c_str() << std::endl;
        std::cout << "  Namespace: " << object.getNamespace().c_str() << std::endl;
        std::cout << "  Object   : " << object.toString().c_str() << std::endl;
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
  std::cout << "Namespace" << std::endl;
  std::cout << "  namespaces            - Shows existing namespaces" << std::endl;
  std::cout << "  use [namespace]       - Sets active namespace" << std::endl;
  std::cout << "  create [namespace]    - Creates a new namespace" << std::endl;
  std::cout << "  destroy [namespace]   - Destroys a namespace" << std::endl;
  std::cout << "Object from Namespace Transport" << std::endl;
  std::cout << "  list <namespace>      - Lists objects in a given namespace" << std::endl;
  std::cout << "  load <object name>   - Load object from namespace" << std::endl;
  std::cout << "Namespace Modification of Currently Loaded Object" << std::endl;
  std::cout << "  save <object name>   - Save object to namespace" << std::endl;
  std::cout << "  rename <new name>    - Renames object in namespace (does not save)" << std::endl;
  std::cout << "  erase <object name>  - Erase object in namespace" << std::endl;
  std::cout << "Currently Loaded Object Attribute Modification" << std::endl;
  std::cout << "  object [object name] - Set the name" << std::endl;
  std::cout << "  set [name=value]     - Sets name=value pair" << std::endl;
  std::cout << "  show                 - Show object's content" << std::endl;
  std::cout << "  clear                - Clear object's content" << std::endl;
  std::cout << "General" << std::endl;
  std::cout << "  status               - Displays current status" << std::endl;
  std::cout << "  x                    - Exits" << std::endl;
}
