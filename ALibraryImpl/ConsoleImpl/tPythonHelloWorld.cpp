#include "APythonEmbed.hpp"
#include "AString.hpp"

// A static module, 'self' is not used 
extern "C" {

static PyObject* afunction(PyObject *self, PyObject* args)
{
  std::cout << "Hello from afunction()" << std::endl;
  return PyInt_FromLong(42L);
}

}

static PyMethodDef amethods[] =
{
	{"afunction",	afunction, METH_VARARGS, "Return the meaning of everything."},
	{NULL, NULL, 0, NULL}		// sentinel 
};

void ainit(void)
{
	PyImport_AddModule("amodule");
	Py_InitModule("amodule", amethods);
}


                                            
main(int argc, char **argv)
{
	
  AString code("\
import sys,amodule\n\
print 'sys.builtin_module_names='+str(sys.builtin_module_names)\n\
print 'sys.modules.keys()='+str(sys.modules.keys())\n\
print 'sys.executable='+str(sys.executable)\n\
print 'sys.argv='+str(sys.argv)\n\
");

  // Pass argv[0] to the Python interpreter 
	Py_SetProgramName(argv[0]);

	// Initialize the Python interpreter.  Required. 
	Py_Initialize();

	// Add a static module 
	ainit();

	// Define sys.argv.  It is up to the application if you
	//   want this; you can also let it undefined (since the Python 
	//   code is generally not a main program it has no business
	//   touching sys.argv...) 
	PySys_SetArgv(argc, argv);

	// Do some application specific code 
	printf("Hello, brave new world\n\n");

	// Execute some Python statements (in module __main__) 
  PyRun_SimpleString(code.c_str());

	// Note that you can call any public function of the Python
	//   interpreter here, e.g. call_object(). 
	PyRun_SimpleString("amodule.afunction()\n");

	// Some more application specific code 
	printf("\nGoodbye, cruel world\n");

	// Exit, cleaning up the interpreter 
	Py_Exit(0);
}
