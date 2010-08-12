/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED_AWordUtility_HPP_
#define INCLUDED_AWordUtility_HPP_

#include "apiABase.hpp"
#include "AString.hpp"

class ABASE_API AWordUtility
{
public:
  /*!
   Approximates the number of syllables in a word
   Split routine creates separate string for each syllable (vector not cleared before adding)
   Returns syllable count
  */
  static int countSyllables(const AString&);
  
  /*!
  Splits a word into syllables and by defualt ignores trailing silent letters (such as e)
  Returns syllable count
  */
  static int splitSyllables(const AString&, VECTOR_AString&, bool handleSilentTrailing = true);

  /*!
  Converts to equivalent sounds-like pattern for comparisson
  result cleared by default
  */
  static void getSoundsLikeForm(const AString& source, AString& result);

  /*!
  Converts to equivalent Soundex pattern for comparisson
  result cleared by default
  This is a very approximate form and resulting form will match a lot of similar sounding words
  minimum size defaults to 4  (set to 0 will have no minimum size)
  */
  static void getSoundexForm(const AString& source, AString& result, size_t minSize = 4);

  /*!
  Conversts to equivalent phonetic form for comparisson
  result cleared by default
  */
  static void getPhoneticForm(const AString& source, AString& result);

  /*!
  Conversts to equivalent phonetic form then does a SoundsLike for comparisson
  result cleared by default
  */
  static void getPhoneticSoundsLikeForm(const AString& source, AString& result);

  /*!
   Checks if two words most likely rhyme
   0  - do not rhyme
   >1 - higher the number the more likely they rhyme
   AConstant::npos means the words sounding identical according to phonetic form
  */
  static size_t getRhymeLevel(const AString& wordOne, const AString& wordTwo);

  /*!
   Tries to make something plural
   Follows simple algorithm good for most words
   There are a lot of exception and at this time they will not be properly pluralized
  */
  static void getPlural(const AString& one, AString& many);

  //a_Public sets
  static const AString sstr_Vowels;                      //a_ ("aeiou")

private:
  static const AString msstr_SyllableSeparators;          //a_ ("aeiouyAEIOUY")
  static const AString msstr_SilentEnd;                   //a_ ("eE")
  static const AString msstr_SoundVowels;                 //a_ ("aeiouy")
};

#endif //INCLUDED_AWordUtility_HPP_
