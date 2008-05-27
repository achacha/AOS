/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__ATime_HPP__
#define INCLUDED__ATime_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include <time.h>

class AString;

/**
 * Design: Wrapper for time_t and RTL fuctions that use tm*
 *         Support basic math and RFC based formatting
 *         All time is assumed UT/GMT/+0000 and setting local time with adjust for timezone
**/
class ABASE_API ATime : public ADebugDumpable
{
public:
  /*!
  Display types
  Time is always stored as UT
  */
  enum eType
  {
    LOCAL = 0x0,
    GMT = 0x1,
    UT = 0x1    //a_GMT and UT are same
  };

  static const ATime GENESIS;   //a_Begining of time (time_t = 0)

public:
  /*!
  By default it is initialized to the time this object was created
  Each time_t struct is time in seconds since Jan 01, 1970
    the releative offset can be used with math operators to get
    time differences and increment time as needed
  */
  ATime(eType e = ATime::GMT);
  ATime(const time_t&, eType e = ATime::GMT);
  ATime(const ATime&);
  ~ATime();

  /*!
  Pure utility function, has little to do with the ATime class
  */
  static size_t getTickCount();
  
  /*!
  High performance timer
    counter - current count
    frequency - counts per second

    Elapsed seconds = (counter1 - counter0) / frequency
  */
  static u8 getHighPerfCounter();
  static u8 getHighPerfFrequency();

  /*!
  Access methods
    setToNow will reset this object to the current tick count
  */
  void setToNow();
  void set(time_t time);
  void set(tm& time);
  operator time_t();
  void gmtime(tm& t);
  void localtime(tm& t);
  
  /*!
  Current effective time zone with DST adjustment
  */
  int getEffectiveTimeZone() const;
  
  /*!
  Is daylight saving time adjusting the current time zone
  */
  bool isDaylightSavingTime() const;

  /*!
  Changing display type
  */
  void setType(eType e);

  /*!
  Output formatting function (just like ANSI)
  NOTE: asctime and ctime do the same thing, included for completeness
  */
  void asctime(AOutputBuffer&) const;
  void ctime(AOutputBuffer&) const;
  void strftime(const AString& strTemplate, AOutputBuffer&) const;
  
  /*!
  AEmittable - emit the tick count
  */
  void emit(AOutputBuffer&) const;

  /*!
  Format usable in logs (in UT or GMT+0)
  YYYYMMDDHHMMSS (e.g. 20060510183240)
  */
  void emitYYYYMMDDHHMMSS(AOutputBuffer&) const;

  /*!
  Format usable in logs (in UT or GMT+0)
  YYYY_MM_DD_HHMMSS (e.g. 2006_05_10_183240)
  */
  void emitYYYY_MM_DD_HHMMSS(AOutputBuffer&) const;

  /*!
  Format usable in filenames based on current day
  YYYYMMDD   (e.g. 20060522)
  */
  void emitYYYYMMDD(AOutputBuffer& target) const;

  /*!
  RFC-822/1036 conversion to
  example: Wdy, DD Mon YYYY HH:MM:SS TIMEZONE
  */
  void emitRFCtime(AOutputBuffer&) const;

  /*
  RFC-822/1036 conversion from
  example: Wdy, DD Mon YYYY HH:MM:SS TIMEZONE
  */
  void parseRFCtime(const AString&);

  /*
  Conversion routines
  */
  double difftime(const ATime&) const;

  /*
  Operators
  */
  ATime& operator  =(const ATime&);
  ATime& operator +=(const ATime&);
  ATime& operator -=(const ATime&);
  
  /*
  Comparisson operators
  */
  bool operator >=(const ATime& t) const;
  bool operator <=(const ATime& t) const;
  bool operator ==(const ATime& t) const;
  bool operator  >(const ATime& t) const;
  bool operator  <(const ATime& t) const;
  bool operator !=(const ATime& t) const;

  /*!
  Helpers functions
  */
  static int getMonthFromString(const AString&);        //a_Returns # for 3 day month (0=Jan, 11=Dec)
  static int getWeekdayFromString(const AString&);      //a_Returns # for day of week (0=Sun, 6=Sat)
  static float getTimeZoneFromString(const AString&);   //a_Returns the corrert offset from UT (and support non-integral offsets)

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  tm* _getTM() const;
  
  //a_This is the internal storage of time
  time_t mt_Time;

  //a_This is the type
  eType me_Type;

  //a_Time zone modifier, usually does not change (unless the computer is always moving?!)
  static int smi_TimeZone;
};

//a_strftime quick reference courtesy of MSDN (http://msdn.microsoft.com)
/*
%a  Abbreviated weekday name
%A  Full weekday name
%b  Abbreviated month name
%B  Full month name
%c  Date and time representation appropriate for locale
%d  Day of month as decimal number (01 - 31)
%H  Hour in 24-hour format (00 - 23)
%I  Hour in 12-hour format (01 - 12)
%j  Day of year as decimal number (001 - 366)
%m  Month as decimal number (01 - 12)
%M  Minute as decimal number (00 - 59)
%p  Current locale’s A.M./P.M. indicator for 12-hour clock
%S  Second as decimal number (00 - 59)
%U  Week of year as decimal number, with Sunday as first day of week (00 - 53)
%w  Weekday as decimal number (0 - 6; Sunday is 0)
%W  Week of year as decimal number, with Monday as first day of week (00 - 53)
%x  Date representation for current locale
%X  Time representation for current locale
%y  Year without century, as decimal number (00 - 99)
%Y  Year with century, as decimal number
%z  Time-zone name or abbreviation; no characters if time zone is unknown (same as %Z)
%Z  Time-zone name or abbreviation; no characters if time zone is unknown (same as %z)
%%  Percent sign

As in the printf function, the # flag may prefix any formatting code. In that case, the meaning of the format code is changed as follows.

Format Code Meaning 
%#a, %#A, %#b, %#B, %#p, %#X, %#z, %#Z, %#% # flag is ignored. 
%#c Long date and time representation, appropriate for current locale. For example: "Tuesday, March 14, 1995, 12:41:29". 
%#x Long date representation, appropriate to current locale. For example: "Tuesday, March 14, 1995". 
%#d, %#H, %#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y Remove leading zeros (if any).
*/

/* RFC-822/1036 excerpt:
5.  DATE AND TIME SPECIFICATION

     5.1.  SYNTAX

     date-time   =  [ day "," ] date time        ; dd mm yy (a_not Y2K compliant using yyyy)
                                                 ;  hh:mm:ss zzz

     day         =  "Mon"  / "Tue" /  "Wed"  / "Thu"
                 /  "Fri"  / "Sat" /  "Sun"

     date        =  1*2DIGIT month 2DIGIT        ; day month year
                                                 ;  e.g. 20 Jun 82

     month       =  "Jan"  /  "Feb" /  "Mar"  /  "Apr"
                 /  "May"  /  "Jun" /  "Jul"  /  "Aug"
                 /  "Sep"  /  "Oct" /  "Nov"  /  "Dec"

     time        =  hour zone                    ; ANSI and Military

     hour        =  2DIGIT ":" 2DIGIT [":" 2DIGIT]
                                                 ; 00:00:00 - 23:59:59

     zone        =  "UT"  / "GMT"                ; Universal Time
                                                 ; North American : UT
                 /  "EST" / "EDT"                ;  Eastern:  - 5/ - 4
                 /  "CST" / "CDT"                ;  Central:  - 6/ - 5
                 /  "MST" / "MDT"                ;  Mountain: - 7/ - 6
                 /  "PST" / "PDT"                ;  Pacific:  - 8/ - 7
                 /  1ALPHA                       ; Military: Z = UT;
                                                 ;  A:-1; (J not used)
                                                 ;  M:-12; N:+1; Y:+12
                 / ( ("+" / "-") 4DIGIT )        ; Local differential
                                                 ;  hours+min. (HHMM)

     5.2.  SEMANTICS

          If included, day-of-week must be the day implied by the date
     specification.

          Time zone may be indicated in several ways.  "UT" is Univer-
     sal  Time  (formerly called "Greenwich Mean Time"); "GMT" is per-
     mitted as a reference to Universal Time.  The  military  standard
     uses  a  single  character for each zone.  "Z" is Universal Time.
     "A" indicates one hour earlier, and "M" indicates 12  hours  ear-
     lier;  "N"  is  one  hour  later, and "Y" is 12 hours later.  The
     letter "J" is not used.  The other remaining two forms are  taken
     from ANSI standard X3.51-1975.  One allows explicit indication of
     the amount of offset from UT; the other uses  common  3-character
     strings for indicating time zones in North America.
*/

#endif
