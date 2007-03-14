
#include <pchAWeb.hpp>
#include <ACookie.hpp>

#undef main
int main(int iArgCount, char **ppcArgValue)
{
  ACookie c;

  c.setNameValue("__id__", "09874309827409874922");
  c.setDomain(".AlexChacha.org");
  c.setMaxAge(360);
  c.setSecure(true);

  cout << c.toString().c_str() << endl;

  return 0x0;
}
