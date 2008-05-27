/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAPythonEmbed.hpp"
#include "APythonExportFunctions.hpp"
#include "AObjectContainer.hpp"
#include "APythonEmbed.hpp"

extern "C" PyObject* PyAObjectContainer_get(PyObject *self, PyObject* args)
{
  AString str;
  const char *pccNS, *pccPath;
  int iNS = 0, iPath = 0;
  if (PyArg_ParseTuple(args, "z#z#", &pccNS, &iNS, &pccPath, &iPath))
  {
    if (pccNS && pccPath)
    {
      AString strNS(pccNS, iNS);
      AString strPath(pccPath, iPath);
      AString result;
      if (APythonEmbed::lookup(strNS, strPath, result))
      {
        return PyString_FromStringAndSize(result.c_str(), result.getSize());
      }
      else
        return PyString_FromStringAndSize("NULL", 4);
    }
    else
      return PyString_FromStringAndSize("None", 4);
  }
  else
    return PyString_FromStringAndSize("ERROR", 5);
}
