/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ATime.hpp"
#include "AException.hpp"
#include "templateAutoPtr.hpp"
#include "AString.hpp"
#include <limits.h>

//FORMAT: Wdy, DD Mon YYYY HH:MM:SS UT+-TIMEZONE
const AString ATime::S_RFC1036("%a, %d-%b-%Y %H:%M:%S UT");
  
const ATime ATime::EPOCH(0);

// Loggable formats
const AString ATime::LOGGABLE_YYYYMMDDHHMMSS("%Y%m%d%H%M%S");
const AString ATime::LOGGABLE_YYYY_MM_DD_HHMMSS("%Y_%m_%d_%H%M%S");
const AString ATime::LOGGABLE_YYYYMMDD("%Y%m%d%");

int ATime::smi_TimeZone(666);       //a_Some unexpected timezone :)

void ATime::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mt_Time=" << mt_Time << std::endl;
  ADebugDumpable::indent(os, indent+1) << "me_Type=" << me_Type << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void ABASE_API ATime::sleep(u4 milliseconds)
{
#ifdef __WINDOWS__
  ::Sleep(milliseconds);
#else
#error Not implemented
#endif
}

u4 ATime::getTickCount32()
{
#ifdef __WINDOWS__
  return ::GetTickCount();
#else
#error Not implemented
#endif
}

u8 ATime::getTickCount()
{
#ifdef __WINDOWS__
  #if WINVER >= 0x0600
    return ::GetTickCount64();
  #else
    return (u8)::GetTickCount();
  #endif
#else
#error Not implemented
#endif
}

u8 ATime::getHighPerfCounter()
{
#ifdef __WINDOWS__
  LARGE_INTEGER largeInt;
  AVERIFY(NULL, ::QueryPerformanceCounter(&largeInt));
  return largeInt.QuadPart;
#else
#error Not implemented
#endif
}

u8 ATime::getHighPerfFrequency()
{
#ifdef __WINDOWS__
  LARGE_INTEGER largeInt;
  AVERIFY(NULL, ::QueryPerformanceFrequency(&largeInt));
  return largeInt.QuadPart;
#else
#error Not implemented
#endif
}

ATime::ATime(ATime::eType e) :
  me_Type(e)
{
  time(&mt_Time);
}

ATime::ATime(const ATime& that) :
  me_Type(that.me_Type),
  mt_Time(that.mt_Time)
{
}

ATime::ATime(const time_t& tt, ATime::eType e) :
  me_Type(e),
  mt_Time(tt)
{ 
}

void ATime::setToNow()
{
  time(&mt_Time);
}

ATime& ATime::operator =(const ATime& that)
{
  me_Type = that.me_Type;
  mt_Time = that.mt_Time;
	return *this;
}

ATime& ATime::operator +=(const ATime& that)
{
  mt_Time += that.mt_Time;
  if (mt_Time < 0x0)
    mt_Time = LONG_MAX;
	return *this;
}

ATime& ATime::operator -=(const ATime& that)
{
  mt_Time -= that.mt_Time;
  if (mt_Time < 0x0)
    mt_Time = 0x0;
	return *this;
}

void ATime::set(time_t time)
{
  mt_Time = time;
}

void ATime::set(tm& time)
{
  mt_Time = mktime(&time);
}

//a_NOTE: object returned from this function must be freed by the caller
//a_      Since gmtime, localtime, and mktime use a single static tm struct,
//a_      we will have a serious multithreading issue if we do not lock and copy
//a_NOTE: returned tm structure must be freed by caller
tm* ATime::_getTM() const
{
  //a_If this is NULL then time is before Jan 1, 1970
  tm* ptmX = new tm;

#if (_MSC_VER >= 1400)
  errno_t errornum = ::gmtime_s(ptmX, &mt_Time);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
#else
  memcpy(ptmX, ::gmtime(&mt_Time), sizeof(tm));
#endif

  return ptmX;
}

void ATime::gmtime(tm& t)
{
#if (_MSC_VER >= 1400)
  errno_t errornum = ::gmtime_s(&t, &mt_Time);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
#else
  memcpy(&t, ::gmtime(&mt_Time), sizeof(tm));
#endif
}

void ATime::localtime(tm& t)
{
#if (_MSC_VER >= 1400)
  errno_t errornum = ::localtime_s(&t, &mt_Time);
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
#else
  memcpy(&t, ::localtime(&mt_Time), sizeof(tm));
#endif
}

ATime::~ATime()
{
}

void ATime::asctime(AOutputBuffer& target) const
{
  if (mt_Time >= 0x0)
  {
#if (_MSC_VER >= 1400)
    char buffer[32]; 
    errno_t errornum = ::ctime_s(buffer, 32, &mt_Time); 
    if (errornum)
      ATHROW_ERRNO(this, AException::APIFailure, errornum);
    target.append(buffer);
#else
    target.append(::ctime(&mt_Time));
#endif
  }
  else
    ATHROW(this, AException::InvalidObject);
}

void ATime::ctime(AOutputBuffer& target) const
{
  if (mt_Time >= 0x0)
  {
#if (_MSC_VER >= 1400)
    char buffer[32]; 
    errno_t errornum = ::ctime_s(buffer, 32, &mt_Time); 
    if (errornum)
      ATHROW_ERRNO(this, AException::APIFailure, errornum);
    target.append(buffer);
#else
    target.append(::ctime(&mt_Time));
#endif
  }
  else
    ATHROW(this, AException::InvalidObject);
}

void ATime::strftime(const AString& strTemplate, AOutputBuffer& target) const
{
  //a_If template is empty do nothing
  if (strTemplate.isEmpty())
    return;

  //a_If there is no time, then it is invalid
  AAutoPtr<tm> ptmX(_getTM(), true);
  if (!ptmX.get())
    ATHROW(this, AException::InvalidObject);

  const size_t MAX_SIZE = 0x800;
  size_t size = 0x200;
  size_t ret;
  AString strBuffer(size, size);
  do
  {
    char *p = strBuffer.startUsingCharPtr();
    ret = (int)::strftime(p, size - 1, strTemplate.c_str(), ptmX.get());
    
    if (!ret)
    {
      strBuffer.reserveMoreSpace(size);
      size <<= 1;
    }

  } while (size <= MAX_SIZE && !ret);

  if (!ret)
  {
    //a_The maximum buffer size is too small for the requested template
    ATHROW(this, AException::ProgrammingError);
  }

  strBuffer.stopUsingCharPtr(ret);
  target.append(strBuffer);
}

void ATime::emitRFCtime(AOutputBuffer& target) const
{
  if (me_Type == ATime::LOCAL)
  {
    ATime t(*this);
    t.mt_Time += getEffectiveTimeZone() * 3600;
    t.strftime(S_RFC1036, target);
  }
  else
    strftime(S_RFC1036, target);

  int iTZ = 0x0;
  switch(me_Type)
  {
    case LOCAL : 
      iTZ = getEffectiveTimeZone();
      if (iTZ >= 0x0)
      {
        if (iTZ < 0xA)
          target.append("+0",2);
        else
          target.append('+');

        target.append(AString::fromInt(iTZ));
      }
      else
      {
        if (iTZ > -0xA)
          target.append("-0",2);
        else
          target.append('-');

        target.append(AString::fromInt(-iTZ));
      }
    break;

    case GMT : 
      //a_Already in UT
    break;

    default :
      ATHROW(this, AException::ProgrammingError);
  }
}

void ATime::parseRFCtime(const AString& strRFCTime)
{
  //a_FORMAT: Wdy, DD Mon YYYY HH:MM:SS TIMEZONE
  //a_Start by finding the weekday
  tm tmNew;
  memset(&tmNew, 0x0, sizeof(tm));

  float fTZ = 0x0;    //a_Timezone adjustment (accounts for nonintegral differentials)
  AString str, strTime = strRFCTime;
  strTime.stripLeading();
  strTime.stripTrailing();
  
  //a_Length of <12 means we have a screwed up time
  //a_should have at least day, month, year
  if (strTime.getSize() < 12)
    ATHROW(this, AException::InvalidParameter);

  //a_First find the comma to see if a weekday is used
  size_t pos = strTime.find(',');

  if (AConstant::npos != pos)
  {
    //a_Weekday
    strTime.get(str, 0x0, pos);
    strTime.stripLeading(", ");
    tmNew.tm_wday = getWeekdayFromString(str);
  }

  //a_Now the next item is the 2 digit DD (day number)
  strTime.get(str, 0x0, 0x2);
  strTime.remove(0x1);
  tmNew.tm_mday = str.toInt();

  //a_3 digit MON (month string)
  strTime.get(str, 0x0, 0x3);
  strTime.remove(0x1);
  tmNew.tm_mon = getMonthFromString(str);

  //a_2 or 4 digit year
  if ( AConstant::npos == (pos = strTime.find(' ')) )
      ATHROW(this, AException::InvalidParameter);

  strTime.get(str, 0x0, pos);
    
  int year = str.toInt();
  if (year < 100)
    tmNew.tm_year =  year;
  else
  {
    //a_Invalid year
    if (year < 1900)
      ATHROW(this, AException::InvalidParameter);
    tmNew.tm_year =  year - 1900;
  }

  //a_If time is present
  if (strTime.getSize() > 0x5)
  {
    //a_2 digit hour
    strTime.remove(0x1);
    strTime.get(str, 0x0, 0x2);
    tmNew.tm_hour = str.toInt();

    //a_2 digit minute
    AASSERT(this, strTime[0x0] == ':');
    strTime.remove(0x1);
    strTime.get(str, 0x0, 0x2);
    tmNew.tm_min = str.toInt();

    //a_2 digit seconds
    if (strTime[0x0] == ':')
    {
      strTime.remove(0x1);
      strTime.get(str, 0x0, 0x2);
      tmNew.tm_sec = str.toInt();
    }

    //a_Now the evils of the time zone
    strTime.remove(0x1);
    if (strTime.getSize() > 0)
      fTZ = getTimeZoneFromString(strTime) - getEffectiveTimeZone();
  }

  set(tmNew);

  //a_Adjust the timezone so that it is stored in UT internally
  AASSERT(this, fTZ > -20.0 && fTZ < 20.0);
  mt_Time -= fTZ * 3600;
}

float ATime::getTimeZoneFromString(const AString& strTimezone)
{
  AString str(strTimezone);
  AString strTemp;
  float fRet;
  char cSign = '+';
  if (str[0x0] == '+' || str[0x0] == '-')
  {
    //a_Must be 5 characters to be valid in this format
    if (str.getSize() != 0x5)
      ATHROW(NULL, AException::InvalidParameter);

    //a_Local differential [+-]HHMM
    cSign = str.get(0x0);
    str.get(strTemp, 0x0, 0x2);
    fRet = float(str.toInt()) / 60.0;
    fRet += strTemp.toInt();
  }
  else
  {
    if (str.getSize() == 0x1)
    {
      //a_Evil military time :)  would be an easy algorithm, had they decided not to use 'j'
      //a_ And now I wonder, why not 'j', 'j' is a nice letter, it means well...
      switch(tolower(str[0x0]))
      {
        case 'z' : fRet = 0.0; break;
        case 'a' : fRet = -1.0; break;
        case 'b' : fRet = -2.0; break;
        case 'c' : fRet = -3.0; break;
        case 'd' : fRet = -4.0; break;
        case 'e' : fRet = -5.0; break;
        case 'f' : fRet = -6.0; break;
        case 'g' : fRet = -7.0; break;
        case 'h' : fRet = -8.0; break;
        case 'i' : fRet = -9.0; break;
        case 'k' : fRet = -10.0; break;
        case 'l' : fRet = -11.0; break;
        case 'm' : fRet = -12.0; break;
        case 'n' : fRet = 1.0; break;
        case 'o' : fRet = 2.0; break;
        case 'p' : fRet = 3.0; break;
        case 'q' : fRet = 4.0; break;
        case 'r' : fRet = 5.0; break;
        case 's' : fRet = 6.0; break;
        case 't' : fRet = 7.0; break;
        case 'u' : fRet = 8.0; break;
        case 'v' : fRet = 9.0; break;
        case 'w' : fRet = 10.0; break;
        case 'x' : fRet = 11.0; break;
        case 'y' : fRet = 12.0; break;
        default:
          ATHROW(NULL, AException::InvalidParameter);
      }
    }
    else
    {
      //a_Time zone specified 
      if (!str.findNoCase("UT"))
      {
        str.remove(0x2);
        fRet = str.toInt();
      }
      else if (!str.findNoCase("GMT"))
      {
        str.remove(0x3);
        fRet = str.toInt();
      } 
      else if (!str.findNoCase("EDT"))
        fRet = -4.0;
      else if (!str.findNoCase("EST") || !str.findNoCase("CDT"))
        fRet = -5.0;
      else if (!str.findNoCase("CST") || !str.findNoCase("MDT"))
        fRet = -6.0;
      else if (!str.findNoCase("MST") || !str.findNoCase("PDT"))
        fRet = -7.0;
      else if (!str.findNoCase("PST"))
        fRet = -8.0;
      else
        ATHROW(NULL, AException::InvalidParameter);
    }
  }

  return fRet;
}

int ATime::getWeekdayFromString(const AString& strWeekday)
{
  switch(strWeekday[0x0])
  {
    case 'M':
    case 'm':
      if      (!strWeekday.findNoCase("MON")) return 0x1;
    break;

    case 'T':
    case 't':
      if      (!strWeekday.findNoCase("TUE")) return 0x2;
      else if (!strWeekday.findNoCase("THU")) return 0x4; 
    break;

    case 'W':
    case 'w':
      if      (!strWeekday.findNoCase("WED")) return 0x3;
    break;

    case 'F':
    case 'f':
      if      (!strWeekday.findNoCase("FRI")) return 0x5;
    break;

    case 'S':
    case 's':
      if      (!strWeekday.findNoCase("SAT")) return 0x6;
      else if (!strWeekday.findNoCase("SUN")) return 0x0;
    break;

    default:
      ATHROW(NULL, AException::InvalidParameter);
  }
  return -1;
}

int ATime::getMonthFromString(const AString& strMonth)
{
  switch(strMonth[0x0])
  {
    case 'J':
    case 'j':
      if      (!strMonth.findNoCase("JAN")) return 0x0;
      else if (!strMonth.findNoCase("JUN")) return 0x5;
      else if (!strMonth.findNoCase("JUL")) return 0x6;
    break;
    
    case 'F':
    case 'f':
      if      (!strMonth.findNoCase("FEB")) return 0x1;
    break;

    case 'M':
    case 'm':
      if      (!strMonth.findNoCase("MAR")) return 0x2;
      else if (!strMonth.findNoCase("MAY")) return 0x4; 
    break;

    case 'A':
    case 'a':
      if      (!strMonth.findNoCase("APR")) return 0x3;
      else if (!strMonth.findNoCase("AUG")) return 0x7;
    break;

    case 'S':
    case 's':
      if      (!strMonth.findNoCase("SEP")) return 0x8;
    break;

    case 'O':
    case 'o':
      if      (!strMonth.findNoCase("OCT")) return 0x9;
    break;

    case 'N':
    case 'n':
      if      (!strMonth.findNoCase("NOV")) return 0xA;
    break;

    case 'D':
    case 'd':
      if      (!strMonth.findNoCase("DEC")) return 0xB;
    break;

    default:
      ATHROW(NULL, AException::InvalidParameter);
  }
  return -1;
}

int ATime::getEffectiveTimeZone() const
{
	if (smi_TimeZone == 666)
  {
#if (_MSC_VER >= 1400)
	  AAutoPtr<tm> ptmGMT(new tm(), true);
    errno_t errornum = ::gmtime_s(ptmGMT.use(), &mt_Time);        //a_Get GMT
    if (errornum)
      ATHROW_ERRNO(this, AException::APIFailure, errornum);

    AAutoPtr<tm> ptmLocal(new tm(), true);
    errornum = ::localtime_s(ptmLocal.use(), &mt_Time);           //a_Get Local
    if (errornum)
      ATHROW_ERRNO(this, AException::APIFailure, errornum);
#else
	  AAutoPtr<tm>   ptmGMT( new tm(*(::gmtime(&mt_Time))) );        //a_Get GMT
    AAutoPtr<tm> ptmLocal( new tm(*(::localtime(&mt_Time))) );     //a_Get Local
#endif

    //a_Account for a day change due to time difference
    int iDiff = ptmLocal->tm_hour - ptmGMT->tm_hour;             //a_Get difference
    if (ptmGMT->tm_mday != ptmLocal->tm_mday)
	  {
		  if (iDiff < 12)
			  iDiff += 24;
		  else
			  iDiff -= 24;
	  }
    smi_TimeZone = iDiff;
  }

  AASSERT(this, smi_TimeZone < 100 && smi_TimeZone > -100);  //a_Sanity check
  return smi_TimeZone;
}

bool ATime::isDaylightSavingTime() const
{
  AAutoPtr<tm> ptmLocal( new tm(), true);
  errno_t errornum = ::localtime_s(ptmLocal.use(), &mt_Time);           //a_Get Local
  if (errornum)
    ATHROW_ERRNO(this, AException::APIFailure, errornum);
  return (ptmLocal->tm_isdst == 0x1 ? true : false);
}

double ATime::difftime(const ATime& that) const
{
  return ::difftime(mt_Time, that.mt_Time);
}

bool ATime::operator >=(const ATime& t) const
{
  return (difftime(t) >= 0.0 ? true : false);
}

bool ATime::operator <=(const ATime& t) const
{
  return (difftime(t) <= 0.0 ? true : false);
}

bool ATime::operator ==(const ATime& t) const
{
  return (t.me_Type == me_Type && t.mt_Time == mt_Time ? true : false);
}

bool ATime::operator >(const ATime& t) const 
{ 
  return !operator<=(t);
}

bool ATime::operator <(const ATime& t) const 
{ 
  return !operator>=(t);
}

bool ATime::operator !=(const ATime& t) const 
{ 
  return !operator==(t);
}

ATime::operator time_t()
{ 
  return mt_Time; 
}

void ATime::emit(AOutputBuffer& target) const
{
  AString::fromSize_t(mt_Time).emit(target);
}

void ATime::emitYYYYMMDDHHMMSS(AOutputBuffer& target) const
{
  if (me_Type == ATime::LOCAL)
  {
    ATime t(*this);
    t.mt_Time += getEffectiveTimeZone() * 3600;
    t.strftime(LOGGABLE_YYYYMMDDHHMMSS, target);
  }
  else
    strftime(LOGGABLE_YYYYMMDDHHMMSS, target);
}

void ATime::emitYYYY_MM_DD_HHMMSS(AOutputBuffer& target) const
{
  if (me_Type == ATime::LOCAL)
  {
    ATime t(*this);
    t.mt_Time += getEffectiveTimeZone() * 3600;
    t.strftime(LOGGABLE_YYYY_MM_DD_HHMMSS, target);
  }
  else
    strftime(LOGGABLE_YYYY_MM_DD_HHMMSS, target);
}

void ATime::emitYYYYMMDD(AOutputBuffer& target) const
{
  if (me_Type == ATime::LOCAL)
  {
    ATime t(*this);
    t.mt_Time += getEffectiveTimeZone() * 3600;
    t.strftime(LOGGABLE_YYYYMMDD, target);
  }
  else
    strftime(LOGGABLE_YYYYMMDD, target);
}

void ATime::setType(eType e)
{
  me_Type = e; 
}
