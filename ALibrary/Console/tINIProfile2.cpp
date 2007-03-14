#include <AINIProfile.hpp>
#include <AString.hpp>

void testSimple()
{
  AINIProfile pm("q:/test000.ini");

  pm.setValue("/Section0", "item00", "value00", FALSE);
  pm.setValue("/Section0", "item01", "value01", FALSE);
  pm.setValue("/Section1", "item10", "value10", FALSE);
  pm.writeNow();
}

void createRemovalTestINI()
{
  AINIProfile pm("q:/removalTest.ini");
  pm.setValue("/Section0", "item00", "value00", FALSE);
  pm.setValue("/Section0", "item01", "value01", FALSE);
  pm.setValue("/Section0/Sub", "item000", "value000", FALSE);
  pm.setValue("/Section0/Sub", "item010", "value000", FALSE);
  pm.setValue("/Section0/Sub", "item020", "value000", FALSE);
  pm.setValue("/Section1", "item10", "value10", FALSE);
  pm.setValue("/Section1", "item11", "value10", FALSE);
  pm.setValue("/Section1", "item12", "value10", FALSE);
  pm.setValue("/Section1/Sub", "item100", "value100", FALSE);
  pm.setValue("/Section1/Sub", "item110", "value100", FALSE);
  pm.setValue("/Section1/Sub", "item120", "value100", FALSE);
  pm.setValue("/Section2", "item20", "value20", FALSE);
  pm.setValue("/Section2", "item21", "value20", FALSE);
  pm.setValue("/Section2", "item22", "value20", FALSE);
  pm.setValue("/Section2/Sub", "item200", "value200", FALSE);
  pm.setValue("/Section2/Sub", "item210", "value200", FALSE);
  pm.setValue("/Section2/Sub", "item220", "value200", FALSE);
  pm.writeNow();
}

void testRemoval()
{
	std::cout << "=======REMOVING!" << std::endl;
  AINIProfile pm("q:/removalTest.ini");
  pm.removeKey("/Section1", true);
  pm.writeNow();
}

#undef main
int main()
{  
  //testSimple();

  createRemovalTestINI();
  testRemoval();

  return 0x0;
}
