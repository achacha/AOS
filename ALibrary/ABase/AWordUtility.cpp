/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AWordUtility.hpp"

const AString AWordUtility::sstr_Vowels("aeiou");

const AString AWordUtility::msstr_SyllableSeparators("aeiouyAEIOUY");
const AString AWordUtility::msstr_SilentEnd("eE");
const AString AWordUtility::msstr_SoundVowels("aeiouy");

int AWordUtility::countSyllables(const AString& source)
{
  if (source.isEmpty())
    return 0;

  int count = 0;
  AString word(source);
  word.stripTrailing(msstr_SilentEnd);

  size_t pos = word.findOneOf(msstr_SyllableSeparators);
  while (AConstant::npos != pos)
  {
    ++count;
    pos = word.findNotOneOf(msstr_SyllableSeparators, pos+1);
    if (AConstant::npos != pos)
      pos = word.findOneOf(msstr_SyllableSeparators, pos+1);
  }

  return count;
}

int AWordUtility::splitSyllables(const AString& source, VECTOR_AString& result, bool handleSilentTrailing /* = true */)
{
  if (source.isEmpty())
    return 0;

  AString word(source);
  
  if (handleSilentTrailing)
    word.stripTrailing(msstr_SilentEnd);

  int count = 0;

  size_t start = 0;
  size_t pos = word.findOneOf(msstr_SyllableSeparators);
  while (AConstant::npos != pos)
  {
    AString str;
    word.peek(str, start, pos - start + 1);
    result.push_back(str.c_str());

    ++count;

    start = pos+1;
    pos = word.findNotOneOf(msstr_SyllableSeparators, start);
    if (AConstant::npos != pos)
      pos = word.findOneOf(msstr_SyllableSeparators, start);
  }

  if (start < word.getSize())
  {
    //a_Append leftovers to last word
    AString str;
    word.peek(str, start);
    if (count > 0)
      result.back().append(str);
    else
      result.push_back(str);
  }

  //a_Account for the silent trailing letters
  if (handleSilentTrailing && word.getSize() < source.getSize())
  {
    AString str;
    source.peek(str, word.getSize());
    if (count > 0)
      result.back().append(str);
    else
      result.push_back(str);
  }

  return count;
}

void AWordUtility::getSoundexForm(const AString& source, AString& result)
{
  result.clear();
  if (source.isEmpty())
    return;

  //a_First character appended as is
  AString str(source);
  str.makeLower();
  size_t sourceSize = str.getSize();

  size_t pos = 0;
  while (pos < sourceSize)
  {
    switch(str.at(pos))
    {
      case 'a':
      case 'e':
      case 'i':
      case 'o':
      case 'u':
      case 'h':
      case 'w':
      case 'y':
        break;
//---      
      case 'p':
        if (pos == 0 && sourceSize > 2)
        {
          if ('s' == str.at(pos+1))
          {
            break;  //a_ps => s at word start
          }
          else if ('h' == str.at(pos+1))
          {
            ++pos; //a_pf => f, fallthrough
          }
        }
      case 'b':
      case 'f':
      case 'v':
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('1');
        }
        else
            result.append('1');
        break;
//---      
      case 'd':
        if (pos+1 < sourceSize)
        {
          if ('g' == str.at(pos+1))
            break;        //a_dg => g
        }                 //a_Fallthough from d to t
      case 't':
        if (pos+1 < sourceSize && 'c' == str.at(pos+1))
        {
          if (pos+2 < sourceSize && 'h' == str.at(pos+2))
          {
            ++pos;
            break;  //a_tch => ch
          }
        }
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('3');
        }
        else
            result.append('3');
        break;
//---      
      case 'g':
        if (pos+1 < sourceSize)
        {
          if ('h' == str.at(pos+1))
          {
            if (pos+2 < sourceSize)
            {
              if ('t' == str.at(pos+2))
              {
                ++pos;
                break;   //a_ght => t
              }
              else
                break;   //a_gh => h
            }
          }
          else if ('n' == str.at(pos+1))
          {
            break;       //a_gn => n
          }
        } //a_Fallthrough

      case 'k':
        if (pos+1 < sourceSize && 'n' == str.at(pos+1))
        {
          break;       //a_kn => n
        }  //a_Fallthrough

      case 'c':
      case 'j':
      case 'q':
      case 's':
      case 'x':
      case 'z':
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('2');
        }
        else
            result.append('2');
        break;
//---      
      case 'l':
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('4');
        }
        else
            result.append('4');
        break;
//---      
      case 'm':
        if ('b' == str.at(pos+1, '\x0'))
        {
          ++pos;  //a_mb => m
        } //a_ Fallthrough
      case 'n':
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('5');
        }
        else
            result.append('5');
//---      
      case 'r':
        if (!result.isEmpty())
        {
          if (str.at(pos) != result.at(result.getSize()-1))
            result.append('6');
        }
        else
            result.append('6');
        break;
    }
    ++pos;
  }

  if (result.getSize() < 4)
    result.setSize(4, '0');
}

size_t AWordUtility::getRhymeLevel(const AString& wordOne, const AString& wordTwo)
{
  //a_Check if one or both are empty
  if (wordOne.isEmpty() || wordTwo.isEmpty())
    return 0;

  AString endOne, soundOne;
  AString endTwo, soundTwo;
  AWordUtility::getPhoneticForm(wordOne, endOne);
  AWordUtility::getPhoneticForm(wordTwo, endTwo);
  AWordUtility::getSoundsLikeForm(endOne, soundOne);
  AWordUtility::getSoundsLikeForm(endTwo, soundTwo);
  
  std::cout << endOne << ":" << endTwo << "=";
  std::cout << soundOne << ":" << soundTwo << std::endl;

  soundOne.reverse();
  soundTwo.reverse();

  size_t level = (soundOne.getSize() > soundTwo.getSize() ? soundOne.diff(soundTwo) : soundTwo.diff(soundOne));
  return level;
}

/*
  Sound conversions
Pre-process
  Before   After
  ght      t

Process ending
  Before   After
  e        (remove)
  ie       i
  ng       n
  y        i

Process
  Before   After
  ch       C
  ck       k
  ea       I
  gh       g
  gn       n
  ie       i
  kh       k
  kn       n
  ks       x
  mb       m
  nc       nk
  oo       U
  ou       o
  ph       f
  ps       s
  q(~u)    k    (q without u)
  qu       kw
  sh       S
  tia      Sa
  th       Z
  wh       w
  ugh      f
  zh       Z

Post-process (@-any vowel)
  ang@     anj@


Phonetics:
 ch = C  (chair = Cair)
 sh = S  (bash = baS)
 zh = Z  (zhivago = Zivago)
 oo = U  (boot = bUt)
*/
void AWordUtility::getPhoneticForm(const AString& source, AString& result)
{
  AString work(source);
  AString temp;
  work.makeLower();

  const char IGNORE_CHAR = '_';
  u4 pos = 0;
  result.clear();
  size_t workSize = work.getSize();
  if (!workSize)
    return;
  
  //a_STEP 1
  //a_Preprocess (before and after data must be same size)
  //a_These replacements take precedence
  int i;
  const int iiPreCount = 1;
  const AString preBefore[iiPreCount] = { "ght" };
  const AString preAfter[iiPreCount]  = { "t__" };
  for (i=0; i<iiPreCount; ++i)
    work.replace(preBefore[i], preAfter[i]);

  //a_STEP 2
  //a_Ending (data in reverse) (before and after data must be same size)
  const int iiEndingCount = 6;
  const AString endBefore[iiEndingCount] = {  "e", "yc", "gn", "uae", "y", "eu" };
  const AString endAfter[iiEndingCount]  = {  "_", "is", "_n", "__o", "i", "_i" };
  work.reverse();
  for (i=0; i<iiEndingCount; ++i)
  {
    if (0 == work.find(endBefore[i]))
    {
      work.overwrite(0, endAfter[i]);
    }
  }
  work.reverse();

  //a_STEP 3
  //a_Iterate and process sounds
  pos = 0;
  while(pos < workSize)
  {
    switch(work.at(pos))
    {
      //a_ ch->C
      case 'c':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('C');
          ++pos;
        }
        else if ('k' == work.at(pos+1, '\x0'))
        {
          temp.append('k');
          ++pos;
        }
        else
          temp.append('c');

      break;

      //a_ ea->I
      case 'e':
        if ('a' == work.at(pos+1, '\x0'))
        {
          temp.append('I');
          ++pos;
        }
        else
          temp.append('e');

      break;

      //a_ gh->g, ght->t, gn->n
      case 'g':
        if ('n' == work.at(pos+1, '\x0'))
        {
          temp.append('n');
          ++pos;
        }
        else if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('g');
          ++pos;
        }
        else
          temp.append('g');
      break;

      //a_ ie->i
      case 'i':
        if ('e' == work.at(pos+1, '\x0'))
        {
          temp.append('i');
          ++pos;
        }
        else
          temp.append('i');

      break;

      //a_kh->k, kn->n, ks->x
      case 'k':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('k');
          ++pos;
        }
        else if ('n' == work.at(pos+1, '\x0'))
        {
          temp.append('n');
          ++pos;
        }
        else
          temp.append('k');
      break;

      //a_nc->nk
      case 'n':
        if ('c' == work.at(pos+1, '\x0'))
        {
          temp.append("nk");
          ++pos;
        }
        else
          temp.append('n');
      break;

      //a_ qu->kw
      case 'm':
        if ('b' == work.at(pos+1, '\x0'))
        {
          temp.append('m');
          ++pos;
        }
        else
          temp.append('m');
      break;

      //a_ oo->U
      case 'o':
        if ('o' == work.at(pos+1, '\x0'))
        {
          temp.append('U');
          ++pos;
        }
        else if ('u' == work.at(pos+1, '\x0'))
        {
          temp.append('o');
          ++pos;
        }
        else
          temp.append('o');
      break;

      //a_ ph->f, ps->s
      case 'p':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('f');
          ++pos;
        }
        else if ('s' == work.at(pos+1, '\x0'))
        {
          temp.append('s');
          ++pos;
        }
        else
          temp.append('p');
      break;

      //a_ qu->kw
      case 'q':
        if ('u' == work.at(pos+1, '\x0'))
        {
          temp.append("kw", 2);
          ++pos;
        }
        else
          temp.append('k');
      break;

      //a_ sh->S
      case 's':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('S');
          ++pos;
        }
        else
          temp.append('s');
      break;

      //a_ th->s, tia->Sa
      case 't':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('Z');
          ++pos;
        }
        else if ('i' == work.at(pos+1, '\x0') && 'o' == work.at(pos+2, '\x0'))
        {
          temp.append("Su");
          pos += 2;
        }
        else
          temp.append('t');
      break;

      //a_ wh->w
      case 'w':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('w');
          ++pos;
        }
        else
          temp.append('w');
      break;

      //a_ zh->Z
      case 'z':
        if ('h' == work.at(pos+1, '\x0'))
        {
          temp.append('Z');
          ++pos;
        }
        else
          temp.append('z');
      break;

      //a_Skip underscores
      case IGNORE_CHAR:
        break;

      default:
        temp.append(work.at(pos));
    }
    
    ++pos;
  }

  //a_STEP 4
  //a_Remove duplicates (by insering underscore which will be ignored later)
  workSize = temp.getSize();
  pos = 0;
  while(pos < workSize)
  {
    if (temp.at(pos) == temp.at(pos+1, '\x0'))
      temp.use(pos) = IGNORE_CHAR;
    ++pos;
  }

  //a_STEP 6
  //a_Post process
  pos = 0;
  workSize = temp.getSize();
  while(pos < workSize)
  {
    switch(temp.at(pos))
    {
      case 'a':
        if ('n' == temp.at(pos+1, '\x0') && 'g' == temp.at(pos+2, '\x0'))
        {
          char c = temp.at(pos+3,'\x0');
          if (c && AConstant::npos != msstr_SoundVowels.find(c))
          {
            result.append("anj", 3);
            pos += 2;
          }
        }
        else
          result.append('a');
      break;

      case IGNORE_CHAR:
        break;

      default:
        result.append(temp.at(pos));
    }

    ++pos;
  }
}

/*
Distinct sound groups
  bfpvw
  mn
  sxz
  ckq
  gj
  dt
  l
  r

Vowels and ignored
  aeiouyh
*/
void AWordUtility::getSoundsLikeForm(const AString& source, AString& result)
{
  result.clear();
  if (source.isEmpty())
    return;

  //a_First character appended as is
  AString work(source);
  work.makeLower();
  size_t workSize = work.getSize();

  size_t pos = 0;
  while (pos < workSize)
  {
    switch(work.at(pos))
    {
      case 'b':
      case 'f':
      case 'p':
      case 'v':
      case 'w':
        result.append('1');
      break;

      case 'm':
      case 'n':
        result.append('2');
      break;

      case 's':
      case 'x':
      case 'z':
        result.append('3');
      break;

      case 'c':
      case 'k':
      case 'q':
        result.append('4');
      break;

      case 'g':
      case 'j':
        result.append('5');
      break;

      case 'd':
      case 't':
        result.append('6');
      break;

      case 'l':
        result.append('7');
      break;
    }
    ++pos;
  }
}

void AWordUtility::getPlural(const AString& one, AString& many)
{
  many.clear();
  
  //a_Words of size 1 or 2, just append s and return
  if (one.getSize() < 3)
  {
    many.assign(one);
    many.append('s');
    return;
  }
  
  switch(one.last())
  {
    case 's':
    {
      char c = one.at(one.getSize()-2);
      if ('i' == c)
      {
        //a_"is" -> "es"
        many.assign(one);
        many.set('i', many.getSize()-2);
      }
      else if ('u' == c)
      {
        //a_"us" -> "ii"
        one.peek(many, 0, one.getSize()-2);
        many.append("ii", 2);
      }
      else
      {
        many.assign(one);
        many.append("es", 2);
      }
    }
    break;
    
    case 'z':
    case 'x':
      many.assign(one);
      many.append("es", 2);
    break;

    case 'h':
    {
      char c = one.at(one.getSize()-2);
      if ('s' == c || 'c' == c)
      {
        many.assign(one);
        many.append("es", 2);
      }
      else
      {
        many.assign(one);
        many.append('s');
      }
    }
    break;

    case 'y':
    {
      char c = one.at(one.getSize()-2);
      if (AConstant::npos != sstr_Vowels.find(c))
      {
        //a_vowel+'y', add 's'
        many.assign(one);
        many.append('s');
      }
      else
      {
        //a_consonant+'y', convert 'y' to 'ies'
        one.peek(many, 0, one.getSize()-1);
        many.append("ies", 3);
      }
    }
    break;

    default:
      many.assign(one);
      many.append('s');
    break;
  }
  
}

