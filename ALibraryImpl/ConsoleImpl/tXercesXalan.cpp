#include "apiAXsl.hpp"
#include "AXsl.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AException.hpp"

int main()
{
  AXsl xslt;
  AString strXml("\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<admin>\
	<object name=\"/classified/classified_submit\" display=\"/classified/classified_submit\">\
		<property name=\"enabled\">\
			<value>true</value>\
			<action name=\"Update\" desc=\"Enable(1) or Disable(0) the command\">\
				<param>enabled</param>\
			</action>\
		</property>\
		<property name=\"session\">\
			<value>false</value>\
			<action name=\"Update\" desc=\"Enable(1) or Disable(0) the command\">\
				<param>enabled</param>\
			</action>\
		</property>\
		<property name=\"ajax\">\
			<value>false</value>\
			<action name=\"Update\" desc=\"Enable(1) or Disable(0) the command\">\
				<param>enabled</param>\
			</action>\
		</property>\
	</object>\
</admin>");
  
  ARope rope1, rope2, rope3;
  try
  {
    xslt.Transform(strXml, ASWNL("c:\\code\\_debug\\testXX.xsl"), rope1);
    std::cout << rope1 << std::endl;
    xslt.Transform(strXml, ASWNL("c:\\code\\_debug\\testXX.xsl"), rope2);
    std::cout << rope2 << std::endl;                                   
    xslt.Transform(strXml, ASWNL("c:\\code\\_debug\\testXX.xsl"), rope3);
    std::cout << rope3 << std::endl;
  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception." << std::endl;
  }

  return 0;
}
