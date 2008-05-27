/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchABase.hpp"
#include "AFragmentString.hpp"
#include "AFragmentConstant.hpp"
#include "AFragmentCounter.hpp"
#include "AFragmentSet.hpp"
#include "AFragmentOdometer.hpp"

void AFragmentString::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AFragmentString @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Finished=" << m_Finished << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Container={" << m_Finished << std::endl;
  for (size_t iX = 0; iX < m_Container.size(); iX++)
    m_Container[iX]->debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AFragmentString::AFragmentString(const AString& str) :
  m_Finished(false)
{
  parse(str);
}

AFragmentString::~AFragmentString()
{
  try
  {
    for (size_t iX = 0; iX < m_Container.size(); iX++)
      delete m_Container[iX];
  }
  catch(...) {}
}

void AFragmentString::clear()
{
  size_t iS = m_Container.size();
  for (size_t iX = 0; iX < iS; iX++)
    delete m_Container[iX];
  m_Container.clear();

  m_Finished = false;
}

void AFragmentString::reset()
{
  size_t iS = m_Container.size();
  for (size_t iX = 0; iX < iS; iX++)
    m_Container[iX]->reset();

  m_Finished = false;
}

bool AFragmentString::next(AOutputBuffer& target)
{
  if (m_Finished)
    reset();

  // Generate output from fragments
  for (size_t i=0; i<m_Container.size(); ++i)
    m_Container[i]->emit(target);

  // Advance the fragments
  int iS = (int)m_Container.size()-1;
  bool bMore = false;
  while (iS >= 0) {
    bMore = m_Container[iS]->next();
    if (!bMore)
      iS--;
    else
      break;
  }

  if ((!bMore) && (iS == -1))
    m_Finished = true;

  return !m_Finished;
}

void AFragmentString::emit(AOutputBuffer& target) const
{
  // Generate output from fragments
  for (size_t i=0; i<m_Container.size(); ++i)
    m_Container[i]->emit(target);
}


size_t AFragmentString::getPermutations() const
{
  double dP = 1;
  size_t iS = m_Container.size();
  for (size_t iX = 0; iX < iS; iX++) {
    dP *= m_Container[iX]->getSetSize();
    if (dP > MAX_PERMUTATIONS)
      return AConstant::npos;
  }
  return (size_t)dP;
}

size_t AFragmentString::parse(const AString& str)
{
  size_t iRet = AConstant::npos;
  size_t iX = 0;
  AString strHold;
  while(iX < str.getSize())
  {
    switch(str[iX])
    {
      case '\\' :
      { 
        ++iX;
        if (str.getSize() > iX)
        {
          strHold += str[iX];
          iRet = iX;
        }
        else
        {
          //a_String truncated
          iRet = iX - 1;
          return iRet;
        }
      }
      break;

      case '(' :
      {
        iX++;
        if (str.getSize() > iX)
        {
          size_t iF = str.find(')', iX+1);
          if (iF != AConstant::npos)
          {
            AASSERT(this, iF >= iX);
            if (!strHold.isEmpty()) { m_Container.push_back(new AFragmentConstant(strHold)); strHold.clear(); iRet = iX-1; }
            AString strT;
            str.peek(strT, iX, iF-iX);
            iX = iF;  // advance offset after extraction

            u1 b;
            u4 n[2];
            
            b = (u1)strT.toInt();
            if (b > 9)
              b = 9;
            if (b < 0)
              b = 0;
            if ((iF = strT.find(',')) == AConstant::npos)
            {
              // Only 1 number, the digit value
              long stop = 10;
              for (int i=1; i<b; ++i)
                stop *= 10;
              m_Container.push_back(new AFragmentCounter(b, u4(stop-1)));
            }
            else
            {
              AString str1;
              strT.peek(str1, iF + 1);
              if ((iF = str1.find(',')) == AConstant::npos)
              {                             
                n[0] = str1.toU4();
                m_Container.push_back(new AFragmentCounter(b, n[0]));
              }
              else
              {
                AString strQ;
                str1.peek(strQ, 0, iF);
                n[0] = strQ.toU4();
                strQ.clear();
                str1.peek(strQ, iF+1);
                n[1] = strQ.toU4();
                if ((iF = str1.find(',', iF+1)) == AConstant::npos)
                  m_Container.push_back(new AFragmentCounter(b, n[0], n[1]));
                else {
                  strQ.clear();
                  str1.peek(strQ, iF+1);
                  m_Container.push_back(new AFragmentCounter(b, n[0], n[1], strQ.toInt()));
                }
              }
            }
            iRet = iX;
          }
          else 
          {
            //a_Closing tag not found
            iRet = iX-1;
            return iRet;
          }
        }
        else
        {
          //a_String is truncated
          iRet = iX-1;
          return iRet;
        }
      }
      break;

      case '{' :
      {
        ++iX;
        if (str.getSize() > iX)
        {
          size_t iF = str.find('}', iX+1);
          if (!strHold.isEmpty()) { m_Container.push_back(new AFragmentConstant(strHold)); strHold.clear(); iRet = iX-1;}
          if (iF != AConstant::npos)
          {
            AASSERT(this, iF >= iX);
            AString strT;
            str.peek(strT, iX, iF-iX);
            char cX;
            while (!strT.isEmpty())
            {
              cX = strT.get();
              switch (cX)
              {
                case '#': m_Container.push_back(new AFragmentSet(AFragmentSet::Numeric)); break;
                case '&': m_Container.push_back(new AFragmentSet(AFragmentSet::LowercaseAlpha)); break;
                case '@': m_Container.push_back(new AFragmentSet(AFragmentSet::UppercaseAlpha)); break;
                case '%': m_Container.push_back(new AFragmentSet(AFragmentSet::LowercaseAlphaNumeric)); break;
                case '^': m_Container.push_back(new AFragmentSet(AFragmentSet::UppercaseAlphaNumeric)); break;
                case '?': m_Container.push_back(new AFragmentSet(AFragmentSet::AlphaNumeric)); break;
                default : strHold += cX;
              }
            }
            iX = iF;
            iRet = iX;
          }
          else
          {
            //a_Closing tag not found
            iRet = iX-1;
            return iRet;
          }
        }
        else
        {
          //a_String is truncated
          iRet = iX-1;
          return iRet;
        }
      }
      break;

      case '[' :
      {
        size_t iF = str.find(']', iX+1);
        if (iF != AConstant::npos)
        {
          AASSERT(this, iF >= iX);
          if (!strHold.isEmpty()) { m_Container.push_back(new AFragmentConstant(strHold)); strHold.clear(); }
          AString strT;
          str.peek(strT, iX + 1, iF - iX - 1);
          m_Container.push_back(new AFragmentSet(strT));
          iX = iF;
          iRet = iX;
        }
        else
        {
          //a_Closing tag not found, return iRet at point of error
          iRet = iX;
          return iRet;
        }
      }
      break;

      case '<' :
      {
        size_t iF = str.find('>', iX+1);
        if (iF != AConstant::npos)
        {
          AASSERT(this, iF >= iX);
          if (!strHold.isEmpty()) { m_Container.push_back(new AFragmentConstant(strHold)); strHold.clear(); }
          AString strT;
          str.peek(strT, iX + 1, iF - iX - 1);
          m_Container.push_back(new AFragmentOdometer(strT.toSize_t()));
          iX = iF;
          iRet = iX;
        }
        else
        {
          //a_Closing tag not found, return iRet at point of error
          iRet = iX;
          return iRet;
        }
      }
      break;

      default  : 
        strHold += str[iX];
        iRet = iX;
    }
    iX++;
  }

  if (!strHold.isEmpty()) {
    m_Container.push_back(new AFragmentConstant(strHold));
  }

  //a_If we got here we have succeeded
  iRet = iX;
  m_Finished = false;

  return iRet;
}

