/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AString_HPP__
#define INCLUDED__AString_HPP__

#include "apiABase.hpp"
#include "AConstant.hpp"
#include "ASerializable.hpp"
#include "AOutputBuffer.hpp"
#include "AXmlEmittable.hpp"
#include "ABase.hpp"
#include "ARandomAccessBuffer.hpp"

class ABASE_API AFile;
class ABASE_API ARope;
class ABASE_API AString;

//a_Containers that involve AString
typedef std::set<AString>               SET_AString;
typedef std::list<AString>              LIST_AString;
typedef std::vector<AString>            VECTOR_AString;
typedef std::map<AString, AString>      MAP_AString_AString;
typedef std::multimap<AString, AString> MMAP_AString_AString;

/*!
Macros to help write faster code using AString with existing const char*
It will call AString::wrap(const char *, size_t) which warps the const char* with a const AString& and supressed an implcit creation of AString class
Needs the length so that strlen() is not called an extra time for constant string
Can always use ASW("foo", AConstant::npos) which will force strlen to be called but should be avoided to improve performance

EXAMPLE:

Given a sample class:

  class MyClass
  {
    static void myFunc(const AString& foo);
  };

OLD WAY:

  MyClass.myFunc("foo");   // Implcitly generates AString and allocates memory for "foo"

FAST WAY:

  MyClass.myFunc(ASW("foo", 3));  // Wraps the const char * and does not allocate anything

NOTE:
ASW   - wraps 'const char *' given a length, best performance but you must know the length at compile time
ASWNL - calls strlen() on the 'const char *' which is not as efficient but more convenient when performance is not an issue
*/
#define ASW(s,l) AString::wrap(s,l)
#define ASWNL(s) AString::wrap(s, AConstant::npos)

/*!
 Generic string class that can contain any arbitrary data that can be represented with a const char * and a length.
 It may contain NULLs as it is a generic buffer class.
 Will reallocate memory as needed so if possible use the AString(size_t, u2) ctor to minimize reallocations
 clear() only resets length to 0 and does not release any buffer memory (for efficiency), use clear(true) to release also
*/
class ABASE_API AString : 
  public ASerializable, 
  public AOutputBuffer,
  public AXmlEmittable,
  public ARandomAccessBuffer
{
public:
  /*!
  Default constructor
  */
  AString();

  /*!
  If constructed with a length of -1, strlen will be used.
  */
  AString(const char *, size_t length = AConstant::npos);
  
  /*!
  Constructed from AEmittable
  */
  AString(const AEmittable&);

  /*!
  Copy constructor
  */
  AString(const AString&);

  /*!
  AString from a single character
  */
  explicit AString(const char);

  /*!
  Specialized version of the constructor used for optimizing
  the manipulation of this string
  */
  explicit AString(size_t length, u2 increment);

  /*!
  Constructor that accepts wide-char in Unicode and stores as UTF8
  */
  explicit AString(const wchar_t* pwccSource, size_t length = AConstant::npos);

  /*!
  Wrap const char* with a const AString, buffer will NOT be released when dtor is called
  This will allow the read-only utility of this class without extra allocation

  Macros ASW and ASWNL are used to wrap a call to this (until C++x0 is implemented)

  @see ASW
  @see ASWNL
  */
  static const AString wrap(const char *pccSource, size_t length = AConstant::npos);

  /*!
  Releases memory stored by this object (unless it was wrapped, the do nothing)
  */
  virtual ~AString();

  /*!
  Returns a const char * to the data stored
  This will never be NULL and will return an empty "" string by default (mc_Null)
  */
  const char *c_str() const { return (m_Length ? mp_Buffer : &mc_Null); }

  /*!
  AString accessors toString returns a copy, emit appends contents
  */
  virtual void emit(AOutputBuffer&) const;
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  Non modifying operators
  */
  AString operator+ (const AString& strSource) const;
  AString operator+ (const char *pccSource) const;
  AString operator+ (char cSource) const;

  /*!
  Modifying operators
  */
  AString& operator=  (const AString& strSource);
  AString& operator= (const char * pccSource);
  AString& operator= (const char cSource);
  AString& operator+= (const AString& strSource);
  AString& operator+= (const char * pccSource);
  AString& operator+= (const char cSource);

  /*!
  Testing operators
    operator <    - less than
    operator >    - greater than
    operator <=   - less that or equal
    operator >=   - greater than or equal
    operator ==   - equal
    operator !=   - not equal
  */
  bool operator<  (const AString& strSource) const;
  bool operator>  (const AString& strSource) const;
  bool operator<= (const AString& strSource) const;
  bool operator>= (const AString& strSource) const;
  bool operator== (const AString& strSource) const;
  bool operator!= (const AString& strSource) const;
  bool operator<  (const char *pccSource)    const;
  bool operator>  (const char *pccSource)    const;
  bool operator<= (const char *pccSource)    const;
  bool operator>= (const char *pccSource)    const;
  bool operator== (const char *pccSource)    const;
  bool operator!= (const char *pccSource)    const;

  /*!
  Conversion utilities to AString
  */
  static AString fromInt(int value);                           //os integer      
  static AString fromBool(bool value);                         //true/false value
  static AString fromS1(s1 value, int base = 0xA);            //1 byte signed   
  static AString fromU1(u1 value, int base = 0xA);            //1 byte unsigned 
  static AString fromS2(s2 value, int base = 0xA);            //2 byte signed   
  static AString fromU2(u2 value, int base = 0xA);            //2 byte unsigned 
  static AString fromS4(s4 value, int base = 0xA);            //4 byte signed   
  static AString fromU4(u4 value, int base = 0xA);            //4 byte unsigned 
  static AString fromS8(s8 value, int base = 0xA);            //8 byte signed 
  static AString fromU8(u8 value, int base = 0xA);            //8 byte unsigned 
  static AString fromSize_t(size_t value, int base = 0xA);    //STL site_t
  static AString fromPointer(const void *value);               //Return 0xXXXXXXXX string from address
  
  /*!
  double
    use precision=-1 to get all digits after decimal point
    forcePrecision will append 0 if not enough digits
  */
  static AString fromDouble(double value, int precision = 3, bool forcePrecision = false);

  /*!
  Parses value and assigns to string
  */
  void parseInt(int value);                      //os integer
  void parseS4(s4 value, int base = 0xA);       //4 byte signed
  void parseU4(u4 value, int base = 0xA);       //4 byte unsigned
  void parsePointer(const void *value);          //a_0xXXXXXXXX string from address

  /*!
  Alignment
  if current string is <size, then alignment wil be done, else nothing will be done
  */
  void justifyRight(size_t size, char pad = ' ');
  void justifyCenter(size_t size, char pad = ' ');
  void justifyLeft(size_t size, char pad = ' ');

  /*!
  Single Unicode char (2 byte) to UTF8 string (1-3 chars depending on encoding)
  */
  static AString fromUnicode(wchar_t value);    //a_Returns UTF-8 encoded string

  /*!
  Convert to int
  
  @return int of the string
  */
  int toInt() const;

  /*!
  Convert to bool
  TRUE: "1" OR no case of "true" OR no case of "yes"
  FALSE: all else

  @return bool of the string
  */
  bool toBool() const;

  /*!
  Convert to signed 4 byte
  Follows strtol format if base is other than 10, signed 4 byte

  @param base of the number
  @return s4 of the string
  */
  s4 toS4(int base = 0xA) const;
  
  /*!
  Convert to unsigned 4 byte
  Follows strtoul format if base is other than 10, unsigned 4 byte

  @param base of the number
  @return u4 of the string
  */
  u4 toU4(int base = 0xA) const;
  
  /*!
  Convert to signed 8 byte
  Follows strtol format if base is other than 10, signed 4 byte

  @param base of the number
  @return s8 of the string
  */
  s8 toS8(int base = 0xA) const;

  /*!
  Convert to unsigned 8 byte
  Follows strtoul format if base is other than 10, unsigned 4 byte

  @param base of the number
  @return u8 of the string
  */
  u8 toU8(int base = 0xA) const;

  /*!
  Convert to unsigned size_t type
  Follows strtoul format if base is other than 10, unsigned 4 byte

  @param base of the number
  @return size_t of the string
  */
  size_t toSize_t(int base = 0xA) const;

  /*!
  Convert to double type  

  @return double of the string
  */
  double toDouble() const;

  /*
  Convert to pointer
  Assumes 0xXXXXXXXX and returns pointer to it

  @return pointer of the string
  */
  void *toPointer() const;

  /*!
  Conversion from UTF-8 to Unicode
  Will get the first UTF-8 sequence and convert to Unicode wide-char

  @return wchat_t of the string (treats the data as if it was 2-byte unicode)
  */
  wchar_t toUnicode() const;

  /*!
  Conversion from UTF-8 to Unicode
  Will get the first UTF-8 sequence and convert to Unicode wide-char and return next position
  NOTE: When uStartPos is passed in, the return is the new position
        If the uStartPos == AConstant::npos, it is the end of the sequence (also denoted by a 0x0 return)
        Return of 0x0 and uStartPos != AConstant::npos signifies conversion is invalid (ie. invalid character at that pos, et al)

  @param start position of the 2-byte unicode
  @return wchat_t of the string (treats the data as if it was 2-byte unicode)
  */
  wchar_t toUnicode(size_t& start) const;

  /*!
  Comparisson: right >0 or <0 left, equal if zero
  */
  inline int compare(char cSource) const;

  /*!
  Comparisson: right >0 or <0 left, equal if zero
  */
  int compare(const char *pccSource, size_t length = AConstant::npos) const;

  /*!
  Comparisson: right >0 or <0 left, equal if zero
  */
  int compare(const AString& strSource) const;

  /*!
  Comparisson: right >0 or <0 left, equal if zero
  */
  int compareNoCase(const AString& strSource) const;

  /*!
  Comparisson
  */
  bool equals(const AString& strSource) const;

  /*!
  Comparisson, no case
  */
  bool equalsNoCase(const AString& strSource) const;

  /*!
  Comparisson
  */
  bool equals(const char *pccSource, size_t length = AConstant::npos) const;

  /*!
  Comparisson, no case
  */
  bool equalsNoCase(const char *pccSource, size_t length = AConstant::npos) const;
  
  /*!
  Checks if the string starts with another
  */
  bool startsWith(const AString&) const;

  /*!
  Checks if the string starts with another (ignores case)
  */
  bool startsWithNoCase(const AString&) const;

  /*!
  Checks if the string ends with another
  */
  bool endsWith(const AString&) const;

  /*!
  Checks if the string ends with another (ignores case)
  */
  bool endsWithNoCase(const AString&) const;

  /*!
  Difference
   returns the first position after index at which the strings are different
   if identical npos is returned
  */
  size_t diff(const AString& strSource, size_t startIndex = 0) const;

  /*!
  Assignment
  */
  void assign(char cSource);
  void assign(const char *pccSource, size_t length = AConstant::npos);
  void assign(const unsigned char *pccSource, size_t length = AConstant::npos);
  void assign(const AString& strSource, size_t length = AConstant::npos);
  void assign(const AEmittable& source);
    
  /*!
  Append N times
  */
  void appendN(int repeatN, char cSource);
  void appendN(int repeatN, const char *pccSource, size_t length = AConstant::npos);
  void appendN(int repeatN, const AString& strSource, size_t length = AConstant::npos);

  /*!
  Simple searching methods (forward)
  */
  size_t find(const AString& strSource, size_t startIndex = 0) const;
  size_t find(const char *pccSource, size_t startIndex = 0) const;
  size_t find(char cSource, size_t startIndex = 0) const;
  size_t findNoCase(const AString& strSource, size_t startIndex = 0) const;
  size_t findNoCase(const char *pccSource, size_t startIndex = 0) const;
  size_t findNoCase(char cSource, size_t startIndex = 0) const;

  /*!
  Simple searching methods (reverse)
  */
  size_t rfind(const AString& strSource, size_t startIndex = AConstant::npos) const;
  size_t rfind(const char *pccSource, size_t sourceLength = AConstant::npos, size_t startIndex = AConstant::npos) const;
  size_t rfind(char cSource, size_t startIndex = AConstant::npos) const;
  size_t rfindNoCase(const AString& strSource, size_t startIndex = AConstant::npos) const;
  size_t rfindNoCase(const char *pccSource, size_t startIndex = AConstant::npos) const;
  size_t rfindNoCase(char cSource, size_t startIndex = AConstant::npos) const;

  /*!
  Finding an offset to the first element in or not in a given set
  */
  size_t findOneOf(const AString& strSet, size_t startIndex = 0) const;
  size_t findNotOneOf(const AString& strSet, size_t startIndex = 0) const;

  /*!
  Reverse finding an offset to the first element in a given set
  
  @param charset to use for finding one of
  @return last position of one of the characters in charset or AConstant::npos if not
  */
  size_t rfindOneOf(const AString& charset) const;

  /*!
  Reverse finding an offset to the first element not in a given set

  @param charset to use for finding one NOT of
  @return last position of one of the characters NOT in charset or AConstant::npos if not
  */
  size_t rfindNotOneOf(const AString& charset) const;

  /*!
  Find a pattern inside a string

  \verbatim
  ? = any 1 character
  * = any set of characters
  \? = ? character
  \* = * character
  \\ = \ character
  \endverbatim

  @param strPattern pattern to match
  @param startIndex start position to start searching for a pattern
  @return true if the pattern matches at position startIndex
  */
  bool matchPattern(const AString& strPattern, size_t startIndex = 0) const;

  /*!
  Find first occurance of a pattern match (@see matchPattern)

  @param strPattern pattern to match
  @param startIndex start position to start searching for a pattern
  @return AConstant::npos if not found or absolute index into the string >=startIndex
  */
  size_t findPattern(const AString& strPattern, size_t startIndex = 0) const;

  /*!
  Splits the string based on a delimiter character, empty strings discarded (i.e. 2 delimiters next to each other, leading delimiter, or trailing delimiter)
  strStrip will be used on each resulting string and stripped from front and back (AConstant::ASTRING_EMPTY means don't strip anything)
  bKeepEmpty if true it will keep empty string (2 delimiters next to each other)
  NOTE: Does not clear the list, appends
  */
  size_t split(LIST_AString& slResult, char delimiter = ' ', const AString& strStrip = AConstant::ASTRING_EMPTY, bool bKeepEmpty = false) const;
  size_t split(VECTOR_AString& slResult, char delimiter = ' ', const AString& strStrip = AConstant::ASTRING_EMPTY, bool bKeepEmpty = false) const;
  
  /*!
  Truncate when a given character is found (inclusive, this character will be replaced by end-of-string)
    delimiter - single character to truncate at (quicker)
    strSet - character set at which to truncate
    startIndex - where in the string to start searching for this character
  */
  void truncateAt(char delimiter, size_t startIndex = 0);
  void truncateAtOneOf(const AString& strSet, size_t startIndex = 0);

  /*!
  Overwrite
   startIndex - Insertion starting point
   bytesToOverwrite - Number of bytes to replace after start (may cause the source to grow)
   sourceBytes - bytes to replace with (length of AConstant::npos implies entire string)
  */
  void overwrite(size_t startIndex, const AString& strSource, size_t sourceBytes = AConstant::npos);
  void overwrite(size_t startIndex, size_t bytesToOverwrite, const AString& strSource, size_t sourceBytes = AConstant::npos);

  /*!
  Replace (replaces all instances of one character/string with another)
  */
  void replace(char source, char target, size_t startIndex = 0, size_t numberToReplace = AConstant::MAX_SIZE_T);
  void replace(const AString& source, const AString& dest, size_t startIndex = 0, size_t numberToReplace = AConstant::MAX_SIZE_T);

  /*!
  Insert
   startIndex - Insertion starting point (AConstant::npos imples append)
   sourceStartIndex - offset into the source buffer
   sourceBytes - bytes after the source index to insert (AConstant::npos imples entire source)
  */
  void insert(size_t startIndex, const AString& strSource, size_t sourceStartIndex = 0, size_t sourceBytes = AConstant::npos);

  /*!
  Counts occurences of a character
  */
  size_t count(char cSource, size_t startIndex = 0) const;

  /*!
  Access to the data
  */
  inline u1  at(size_t index) const;
  inline u1  at(size_t index, u1 u1Default) const;       //a_Special form that never throws out-of-bounds exception, returns default when index is out-of-bound
  inline u1  operator[](size_t index) const;
  inline u1& operator[](size_t index);
  inline u1  last() const;                             //a_Last character

  /*!
  Utilities
  */
  void reverse();
  void makeLower();
  void makeUpper();

  /*!
  Strip leading
  */
  AString& stripLeading(const AString& strDelimiters = AConstant::ASTRING_WHITESPACE);
  /*!
  Strip trailing
  */
  AString& stripTrailing(const AString& strDelimiters = AConstant::ASTRING_WHITESPACE);
  
  /*!
  Strip leading and trailing only
  */
  AString& stripLeadingAndTrailing(const AString& strDelimiters = AConstant::ASTRING_WHITESPACE);

  /*!
  Strip entire string
  */
  AString& stripEntire(const AString& strDelimiters = AConstant::ASTRING_WHITESPACE);

  /*!
  Internal helpers
  */
  size_t getHash(size_t upperLimit = AConstant::MAX_SIZE_T) const;      //a_Generates a hash code used by the hash table [0, uUpperLimit)

  /*!
   File based methods
   Will read entire contents of AFile, use AFile methods for finer grain control
  */
  void peekFromFile(AFile& aFile);

  /*!
  peek functions will get the content without removing it
  peek(size_t) will return the byte at a given index
  peek(AString&,size_t,size_t) will append bytes from a given index into another buffer and return bytes appended
  peekUntil(AString&,size_t,const AString&) will copy bytes from a given index until delimiter is found or everything to end if not; returns new position or npos if not found
  peekUntil(AString&,size_t,const AString&) will copy bytes from a given index until one of the delimiters is found or everything to end if not; returns new position or npos if not found
  */
  inline u1 peek(size_t index = 0) const;
  size_t peek(AOutputBuffer& bufDestination, size_t index = 0, size_t bytes = AConstant::npos) const;
  size_t peekUntil(AOutputBuffer& bufDestination, size_t index, char delimiter) const;
  size_t peekUntil(AOutputBuffer& bufDestination, size_t index, const AString& delimiters) const;
  size_t peekUntilOneOf(AOutputBuffer& bufDestination, size_t index = 0, const AString& delimiters = AConstant::ASTRING_WHITESPACE) const;

  /*!
  ARandomAccessBuffer
  Equivalent to peek here
  */
  virtual size_t access(AOutputBuffer& target, size_t index = 0, size_t bytes = AConstant::npos) const;

  /*!
  Indexed access operators
    use(size_t) - will allow modification of the element at offset
  */
  u1& use(size_t index);

  /*!
  get() Gets and removes byte at a given index
  */
  u1 get(size_t index = 0);

  /*!
  rget() Gets and removes last byte
  */
  u1 rget();
  
  /*!
  Get (copy and remove) bytes

  @param target to move data into
  @param sourceIndex to start moving from
  @param bytes to move
  @return AConstant::npos if not found, or number of bytes moved
  */
  size_t get(AString& target, size_t sourceIndex = 0, size_t bytes = AConstant::npos);

  /*!
  Get (copy and remove) bytes from a given index into another buffer until one of the delimiters is found

  @param target to move data into
  @param delimiters to remove until one is found
  @param removeDelimiter if true will remove all delimiters
  @return AConstant::npos if not found, or number of bytes moved
  */
  size_t getUntilOneOf(AString& target, const AString& delimiters = AConstant::ASTRING_WHITESPACE, bool removeDelimiters = true);
  
  /*!
  Copy and remove bytes delimiter is found, delimiter is not part of the result
  
  @param target to move data into
  @param pattern to look for and stop moving (not one of but entire pattern)
  @param removeDelimiter determines if it is removed
  @return # of bytes moved
  */
  size_t getUntil(AString& target, const AString& pattern, bool removeDelimiter = true);

  /*!
  Copy and remove bytes delimiter is found, delimiter is not part of the result
  
  @param target to move data into
  @param delimiter
  @param removeDelimiter determines if it is removed
  @return # of bytes moved
  */
  size_t getUntil(AString& target, char delimiter, bool removeDelimiter = true);

  /*!
  Pop removes count at a position

  @param length to remove
  @param offset to remove at
  */
  void remove(size_t length = 1, size_t offset = 0);

  /*!
  Pops from the end, effectively shrinking the string

  @param length to remove
  */
  void rremove(size_t length = 1);

  /*!
  Remove until a one of the delimiters is found, if not found will clear

  @param delimiters to remove until one is found
  @param removeDelimiter if true will remove all delimiters
  @return AConstant::npos if not found, >=0 otherwise
  */
  size_t removeUntilOneOf(const AString& delimiters = AConstant::ASTRING_WHITESPACE, bool removeDelimiters = true);

  /*!
  Remove until a one of the delimiters is found, if not found will clear

  @param delimiter to remove until
  @param removeDelimiter if true will remove all subsequent found delimiters
  @return AConstant::npos if not found, >=0 otherwise
  */
  size_t removeUntil(char delimiter, bool removeDelimiter = true);

  /*!
  Reverse remove until a given delimiter, if not found will clear

  @param delimiters to remove until one is found
  @param removeDelimiter if true will remove all delimiters
  @return AConstant::npos if not found, >=0 otherwise
  */
  size_t rremoveUntilOneOf(const AString& delimiters = AConstant::ASTRING_WHITESPACE, bool removeDelimiters = true);

  /*!
  Reverse remove until a given delimiter, if not found will clear

  @param delimiter to remove until
  @param removeDelimiter if true will remove all subsequent found delimiters
  @return AConstant::npos if not found, >=0 otherwise
  */
  size_t rremoveUntil(char delimiter, bool removeDelimiter = true);

  /*!
  set(u1,size_t) will set 1 byte at an index (yes, its really a poke! :)
  set(AString&,size_t) sets this buffer with source at an index and grows buffer as needed
  set(AString&,size_t,size_t,size_t) sets this buffer with source at an index and grows buffer as needed and int in length
  */
  void set(u1 uByte, size_t index);
  void set(const AString& bufSource, size_t destinationIndex = 0);
  void set(const AString& bufSource, size_t destinationIndex, size_t bytes, size_t sourceIndex);

  /*!
  Fills entire string with the byte
  */
  void fill(u1 uByte);

	/*!
  Will erase contents and release(free/delete) the buffer if clear(true) is called
  */
  virtual void clear() { clear(false); }    //a_By default, memory is not released
  void clear(bool bReleaseBuffer);
      
	/*!
  Current logical length of the contents
  */
  virtual size_t getSize() const;

	/*!
  Physical size returned which is to be used mostly for information purposes as it may have no bearing on logical buffer size
  */
  size_t getPhysicalSize() const;

  /*!
  Sets the new physical size of the buffer
  Pads the current content with a given byte
  To only allocate the space in the internal buffer use reserve() or reserveMoreSpace() instead
  */
  void setSize(size_t newSize, u1 u1Pad = ' ');

  /*!
  Returns true if the buffer is logically empty
  */
  bool isEmpty() const;

  /*!
  Reserves space for the string only, does not alter content (but may truncate if size is less than actual)
  Reserving more space doesn't mean you can access it, just allocated for future use (useful before doing lots of appends)
  To resize the actual string and pad it use setSize(...) call
  */
  void reserve(size_t size);

  /*!
  Reserves additional space for the string only, does not alter content, just makes sure additional space is allocated
  Reserving more space doesn't mean you can access it, just allocated for future use (useful before doing lots of appends)
  To resize the actual string and pad it use setSize(...) call
  */
  void reserveMoreSpace(size_t moreSpace);
  
  /*!
  Unicode support
    assignDoubleByte - takes a string of double byte Unicode wchars and converts to
      internal storage of UTF-8
      uWCharCount of AConstant::npos implies the use of strlen()
    getDoubleByte - converts the internal string as if it was a UTF-8 and returns a
      double byte version in the given buffer
      allocates the return value and assumes the caller will OWN it
      buffer size is uWCharCount * 2 + 1  (+1 for the NULL termination)
      uWCharCount of AConstant::npos implies length of this
      NOTE: if this is empty return value is NULL
   NOTE: uWCharCount is the # of double byte characters (0.5 of the total buffer length)
    getUTF8Length - calculates the logical length of a UTF-8 string
  */
  void     assignDoubleByte(const wchar_t *pwccSource, size_t wideCharCount = AConstant::npos);
  wchar_t *getDoubleByte(size_t wideCharCount = AConstant::npos) const;
  size_t   getUTF8Length() const;  //a_This is the logical length

  /*!
  Access to internal buffer, use this method at your own risk
  This can be NULL
  */
  const void *data() const;
  
  /*!
  Access actual data
  VERY DANGEROUS and provided for convenience and compatibility
   with older libraries.
   Actual data pointer at this instance.
   DO NOT STORE, changes at allocations or deallocations
   Can be NULL
   
   To use:
   {
     AString str;
     char* pc = str.startUsingCharPtr(16);      //allocate 16 bytes
     strcpy(pc, "Hello");
     size_t newLength = str.stopUsingCharPtr(5);   //effective size is 5 (allocated is still 16)
   }
   
   Please make sure that the buffer is NULL terminated before
   calling stopUsingCharPtr() without length otherwise provide the new length
   so that binary data can be supported (and it can have NULLs in it)
   without provided length strlen() is called
   
   startUsingCharPtr() will not reallocate the buffer and use what already has been allocated
   
   YOU HAVE BEEN WARNED!
  */
  char  *startUsingCharPtr(size_t neededSize = AConstant::npos);
  size_t stopUsingCharPtr(size_t newSize = AConstant::npos);

  /*!
   Internal utilities for this class
  */
  u2   getBufferIncrementSize() const;
  void setBufferIncrementSize(u2 newIncrement);

  /*!
  ASerializable
  */
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Internal buffer used for storage
  When mbool_Wrapper is true, memory is not re/de-allocated
  */
  char *mp_Buffer;
  bool mbool_Wrapped;

  /*!
  Length of the data in the buffer (size_t is AConstant::MAX_SIZE_T)
  */
  size_t m_Length;
  
  /*!
  Resizes the buffer (may do nothing if buffer is larger than needed)
  Always pass desired content length and assume resize will handle the
  internal length correctly
  Sets the terminating NULL
  */
  void _resize(size_t newBufferSize);

  /*!
  Assigns data to the internal buffer
  */
  void _assign(const char * pccSource, size_t length);

  /*!
  Append methods for AOutputBuffer
  */
  virtual size_t _append(const char *pccSource, size_t length);

  /*!
  Compare function returns 0 if equal to string, or +- (result of strncmp or strnicmp) the length of the difference
  */
  int _compare(const char *pccSource, size_t length, bool boolNoCase = false) const;

  /*!
  Find function will locate the buffer/char in this string
  and return the position, npos if not found
  */

  size_t _find(char cSource, size_t startIndex = 0) const;
  size_t _find(const char *pccSource, size_t length, size_t startIndex = 0) const;
  size_t _rfind(char cSource, size_t startIndex = AConstant::npos) const;
  size_t _rfind(const char *pccSource, size_t length, size_t startIndex = AConstant::npos) const;
  size_t _findNoCase(char cSource, size_t startIndex = 0) const;
  size_t _findNoCase(const char *pccSource, size_t length, size_t startIndex = 0) const;
  size_t _rfindNoCase(char cSource, size_t startIndex = AConstant::npos) const;
  size_t _rfindNoCase(const char *pccSource, size_t length, size_t startIndex = AConstant::npos) const;

  /*!
  Removes bytes at a given offset
  */
  void _remove(size_t bytes, size_t sourceIndex);
  
  /*!
  Memory management
  */
  char *_allocate(size_t bytes);
  void  _deallocate(char **ppcBuffer);
    
  /*!
  The concept of NULL does not exist for a string
  object, a non initialized object returns an empty
  string
  */
  static char mc_Null;

  /*!
  Special constructor that wraps AString around a char * but will not delete it
  */
  explicit AString(char *, size_t, bool);

private:
  //a_The size of the entire buffer not the data contained
  size_t m_InternalBufferSize;
  
  //a_The increment of the buffer when data added is less that it
  //a_This is used to accelerate the class by reducing reallocations
  u2 m_BufferIncrement;

  //a_Default allocation block
  static const u2 smi_DefaultBufferIncrement;
};

#endif // INCLUDED__AString_HPP__

