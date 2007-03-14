
#include <AContentType_Form.hpp>

void testFormparse()
{
  AContentType_Form form;

  AString strForm("name=1&somevalue=2&more=3&number=4");
  form.setData(strForm);
  form.parse();

  cout << form.toString() << endl;
}

#undef main
int main()
{
  testFormparse();
  return 0x0;
}