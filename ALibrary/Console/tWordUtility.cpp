#include "AWordUtility.hpp"
#include "AFile_IOStream.hpp"

#define SHOWME(x) std::cout << x << "=" << AWordUtility::countSyllables(x) << std::endl;
void testSyllables()
{
  SHOWME("nutshell");
  SHOWME("temperature");
  SHOWME("ready");
  SHOWME("edition");
  SHOWME("temparment");
  SHOWME("elation");
  SHOWME("gaelic");
  SHOWME("cherry");
  SHOWME("bypass");
  SHOWME("python");
}

void testSoundsLikeForm()
{
  AString str;

  AWordUtility::getSoundsLikeForm("right", str);
  std::cout << str << std::endl;

  AWordUtility::getSoundsLikeForm("write", str);
  std::cout << str << std::endl;

  AWordUtility::getSoundsLikeForm("rite", str);
  std::cout << str << std::endl;

  AWordUtility::getSoundsLikeForm("gnome", str);
  std::cout << str << std::endl;

  AWordUtility::getSoundsLikeForm("nome", str);
  std::cout << str << std::endl;

  AWordUtility::getSoundsLikeForm("knome", str);
  std::cout << str << std::endl;
}

void testSyllableSplit()
{
  size_t i;
  VECTOR_AString vec;
  AString str;
  AWordUtility::splitSyllables("theree", vec);
  for (i=0;i<vec.size();++i)
  {
    AWordUtility::getSoundsLikeForm(vec.at(i), str);
    std::cout << vec.at(i) << "\t=\t" << str << std::endl;
  }

  vec.clear();
}

void testRhyming()
{
  AFile_IOStream ios;
  AString wordOne, wordTwo;
  VECTOR_AString vecOne, vecTwo;
  ios.getOStream() << "Type 'exit' for wordOne to exit." << std::endl;
  while (true)
  {
    ios.getOStream() << "WordOne: ";
    ios.readLine(wordOne);
    if (!wordOne.compare("exit"))
      break;

    ios.getOStream() << "WordTwo: ";
    ios.readLine(wordTwo);

    int level = AWordUtility::getRhymeLevel(wordOne, wordTwo);
    ios.getOStream() << "\n" << wordOne << " and " << wordTwo << " have rhyme level of " << level << std::endl;

    wordOne.clear();
    wordTwo.clear();
  }
}

void testPhoneticForm()
{
  AFile_IOStream ios;
  AString word, result;
  ios.getOStream() << "Type 'exit' to exit." << std::endl;
  while (true)
  {
    ios.getOStream() << "Word: ";
    ios.readLine(word);
    if (!word.compare("exit"))
      break;

    AWordUtility::getPhoneticForm(word, result);
    ios.getOStream() << "\n" << word << " = " << result << std::endl;

    word.clear();
  }
}

void testPlural()
{
  AFile_IOStream ios;
  AString word, result;
  ios.getOStream() << "Type 'exit' to exit." << std::endl;
  while (true)
  {
    ios.getOStream() << "Word: ";
    ios.readLine(word);
    if (!word.compare("exit"))
      break;

    AWordUtility::getPlural(word, result);
    ios.getOStream() << "\n" << word << " = " << result << std::endl;

    word.clear();
  }
}

int main()
{
  //testSyllables();
  //testSoundsLikeForm();
  //testSyllableSplit();
  //testRhyming();
  //testPhoneticForm();
  testPlural();

  return 0;
}
