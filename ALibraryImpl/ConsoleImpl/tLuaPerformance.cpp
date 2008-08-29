#include "apiABase.hpp"
#include "apiALuaEmbed.hpp"
#include "ATemplate.hpp"
#include "ALuaTemplateContext.hpp"
#include "ALuaEmbed.hpp"
#include "AXmlDocument.hpp"
#include "ATemplateNodeHandler_LUA.hpp"
#include "AFile_AString.hpp"
#include "AThread.hpp"

void doLuaTemplate()
{
  AFile_AString code(
"\
%[LUA]{{{ print 'Hello World'; }}}[LUA]%\
");

  AXmlDocument model("root");
  AEventVisitor visitor;
  ABasePtrContainer objects;
  ALuaTemplateContext ctx(objects, model, visitor);

  ATemplate t;
  t.addHandler(new ATemplateNodeHandler_LUA());

  t.fromAFile(code);

  AString output;
  t.process(ctx, output);
  std::cout << output << std::endl;
}

int main(int argc, char **argv)
{
  DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(false);
  
  try
  {
    DEBUG_MEMORY_SET_START_CHECKPOINT();
    for (int t=0; t<10000; ++t)
    {
      doLuaTemplate();
      AThread::sleep(100);
      std::cout << "." << std::flush;
    }
    DEBUG_MEMORY_LEAK_ANALYSIS_END();
  }
  catch(AException& ex)
  {
    std::cout << ex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }
  
  return 1;
}