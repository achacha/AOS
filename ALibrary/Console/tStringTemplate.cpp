#include "ATemplate.hpp"
#include "ABasePtrHolder.hpp"
#include "AXmlDocument.hpp"
#include "AString.hpp"

int main()
{
  AXmlDocument *pModel = new AXmlDocument("sql");
  pModel->useRoot().addElement("table").addData("user");
  pModel->useRoot().addElement("id").addData("11374");

  ABasePtrHolder objects;
  objects.insert(ATemplate::OBJECTNAME_MODEL, pModel);

  ATemplate st("SELECT * FROM %[MODEL]{{{ /sql/table }}}[MODEL]% WHERE ID='%[MODEL]{{{  /sql/id }}}[MODEL]%'");
  //st.debugDump();

  AString output;

  st.process(objects, output);

  std::cout << output << std::endl;
  
  return 0;
}