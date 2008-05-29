/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAPythonEmbed.hpp"
#include "APythonEmbed.hpp"
#include "APythonExportFunctions.hpp"
#include "AObjectContainer.hpp"

APythonEmbed::OBJECTS APythonEmbed::sm_Objects;

static PyMethodDef ALibrary_methods[2] =
{
	{"PyAObjectContainer_get", PyAObjectContainer_get, METH_VARARGS, "Get from global AObjectContainer"},
	{NULL, NULL, 0, NULL}		// sentinel 
};

APythonEmbed::APythonEmbed()
{
}

APythonEmbed::~APythonEmbed()
{
  Py_Finalize();
}

bool APythonEmbed::initialize(char *this_executable)
{
  Py_SetProgramName(this_executable);

  // Initialize the Python interpreter
  Py_Initialize();

  //a_Register ALibrary module
	if (NULL == PyImport_AddModule("ALibrary"))
    return false;

	Py_InitModule("ALibrary", ALibrary_methods);
  
  PyImport_ImportModule("ALibrary");

  return true;
}

void APythonEmbed::execute(const AEmittable& code)
{
  AString str(10240, 10240);
  code.emit(str);
  PyRun_SimpleString(str.c_str());
}

void APythonEmbed::registerObjectContainer(const AString& name, AObjectContainer& ns)
{
  sm_Objects[name] = &ns;
}

bool APythonEmbed::lookup(const AString& name, const AString& path, AOutputBuffer& target)
{
  OBJECTS::const_iterator cit = sm_Objects.find(name);
  if (cit != sm_Objects.end())
  {
    AObjectBase *pObj = (*cit).second->getObject(path);
    if (pObj)
    {
      pObj->emit(target);
      return true;
    }
    else
    {
      target.append("None");
      return false;
    }
  }
  else
  {
    target.append("NULL");
    return false;
  }
}


