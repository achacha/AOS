#include "APythonEmbed.hpp"
#include "AString.hpp"
#include "AObjectContainer.hpp"
#include "AUrl.hpp"

void testPythonCall(char *pcThisMainPath)
{
  static AObjectContainer g_global("global");

  AString code0("\
import sys,ALibrary\n\
print 'sys.builtin_module_names='+str(sys.builtin_module_names)\n\
print 'sys.modules.keys()='+str(sys.modules.keys())\n\
print 'sys.executable='+str(sys.executable)\n\
");

  AString code1("\
import ALibrary;\n\
myname = ALibrary.PyAObjectContainer_get('global', '/name');\n\
mypath = ALibrary.PyAObjectContainer_get('global', '/path');\n\
data = ALibrary.PyAObjectContainer_get('global', '/data');\n\
print 'myname='+myname;\n\
print 'mypath='+mypath;\n\
print '';\n\
print 'data={';\n\
print data;\n\
print '';\n\
print '}';\n\
");

  g_global.insert("/data/code.0", code0);
  g_global.insert("/data/code.1", code1);
  g_global.insert("/path", new AObject<AUrl>(AUrl("http://www.achacha.org/")));
  g_global.insert("/name", "This is my name!");
  
  APythonEmbed python;
  python.registerObjectContainer("global", g_global);

  if (python.initialize(pcThisMainPath))
  {
    python.execute(code1);
  }
}

int main(int argc, char **argv)
{
  testPythonCall(argv[0]);
  return 0;
}
