#include "ABasePtrHolder.hpp"
#include "AxmlDocument.hpp"

int main()
{
  AXmlDocument doc("root");
  doc.useRoot().addElement("foo");

  ABasePtrHolder holder;
  holder.insert("xmldoc", &doc);

  AXmlDocument *pDoc = holder.getAsPtr<AXmlDocument>("xmldoc");
  if (pDoc)
  {
    AString str;
    pDoc->emit(str, 0);
    std::cout << str << std::endl;
  }
  else
  {
    std::cerr << "Unable to cast 'xmldoc' to AXmlDocument*" << std::endl;
  }
}