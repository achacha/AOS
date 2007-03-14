#include <AString.hpp>
#include <ARandomNumberGenerator.hpp>
#include <ATimer.hpp>

static AString strCard("23456789TJQKA");
static AString strSuit("HDCS");

int main()
{
  ATimer timer;
  timer.start();

  AString strCardDeck(52, 4);  //a_1 card, 1 suit
  AString strSuitDeck(52, 4);  //a_1 card, 1 suit

  //a_Create a deck
  for (int iX = 0x0; iX < strSuit.length(); ++iX)
    for (int iY = 0x0; iY < strCard.length(); ++iY)
    {
      strSuitDeck.append(strSuit[iX]);
      strCardDeck.append(strCard[iY]);
    }

  cout << strCardDeck.c_str() << endl;
  cout << strSuitDeck.c_str() << endl;
  
  //a_Now shuffle the deck
  for (iX = 0x0; iX < 6; ++iX)
  {
    int iPos0, iPos1;
    char cTemp;
    for (int iZ = 0x0; iZ < 1024; ++iZ)
    {
      iPos0 = ARandomNumberGenerator::intRange(52, 0);
      while ((iPos1 = ARandomNumberGenerator::intRange(52, 0)) == iPos0);

      //a_Swap cards
      cTemp = strCardDeck[iPos0];
      strCardDeck[iPos0] = strCardDeck[iPos1];
      strCardDeck[iPos1] = cTemp;

      //a_Swap suits
      cTemp = strSuitDeck[iPos0];
      strSuitDeck[iPos0] = strSuitDeck[iPos1];
      strSuitDeck[iPos1] = cTemp;
    }
  
    cout << strCardDeck.c_str() << endl;
    cout << strSuitDeck.c_str() << endl;
  }
 
  timer.stop();
  cout << "\r\nElapsed time=" << AString::fromLong(timer.getInterval()).c_str() << endl;
  return 0x0;
}