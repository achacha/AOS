#include "AWordUtility.hpp"

void doit(const AString& input)
{
  LIST_AString words;
  input.split(words);

  for (LIST_AString::iterator it = words.begin(); it != words.end(); ++it)
  {
    AString tmp;
    AString output;
    AWordUtility::getPhoneticForm(*it, tmp);
    AWordUtility::getSoundsLikeForm(tmp, output);

    std::cout << *it << " : " << tmp << " : " << output << std::endl;
  }
}

int main()
{
  doit("Two large circles");
  doit("Circle too large");
  doit("Another to circle");

  return 0;
}