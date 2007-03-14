#ifndef INCLUDED__AString_HPP__
#define INCLUDED__AString_HPP__

#include "apiABase.hpp"
#include "AConstant.hpp"
#include "ASerializable.hpp"
#include "AOutputBuffer.hpp"

class AFile;
class ARope;
class AString;

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
  public AOutputBuffer
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
  Constructed from a Rope
  */
  AString(const ARope&);

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

  See #define ASW(s,l) above for more information
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
  AString& operator+= (const AString& strSource);
  AString& operator+= (const char * pccSource);
  AString& operator+= (const char cSource);
  AString& operator= (const char * pccSource);
  AString& operator= (const char cSource);

  /*!
  Testing operators
    operator <    - less than
    operator >    - greater than
    operator <=   - less that or equal
    operator >=   - greater than or equal
    operator ==   - equal
    operator !=   - not equal
  */
  bool operator<  (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) <  0 ? true : false); }
  bool operator>  (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) >  0 ? true : false); }
  bool operator<= (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) <= 0 ? true : false); }
  bool operator>= (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) >= 0 ? true : false); }
  bool operator== (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) == 0 ? true : false); }
  bool operator!= (const AString& strSource) const { return (_compare(strSource.mp_Buffer, strSource.m_Length) != 0 ? true : false); }
  bool operator<  (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) <  0 ? true : false); }
  bool operator>  (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) >  0 ? true : false); }
  bool operator<= (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) <= 0 ? true : false); }
  bool operator>= (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) >= 0 ? true : false); }
  bool operator== (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) == 0 ? true : false); }
  bool operator!= (const char *pccSource)    const { return (_compare(pccSource, AConstant::npos) != 0 ? true : false); }

  /*!
  Conversion utilities to AString
  */
  static AString fromInt(int value);                           //os integer      
  static AString fromBool(bool value);                         //true/false value
  static AString fromS1(s1 value, int iBase = 0xA);            //1 byte signed   
  static AString fromU1(u1 value, int iBase = 0xA);            //1 byte unsigned 
  static AString fromS2(s2 value, int iBase = 0xA);            //2 byte signed   
  static AString fromU2(u2 value, int iBase = 0xA);            //2 byte unsigned 
  static AString fromS4(s4 value, int iBase = 0xA);            //4 byte signed   
  static AString fromU4(u4 value, int iBase = 0xA);            //4 byte unsigned 
  static AString fromS8(s8 value, int iBase = 0xA);            //8 byte signed 
  static AString fromU8(u8 value, int iBase = 0xA);            //8 byte unsigned 
  static AString fromSize_t(size_t value, int iBase = 0xA);    //STL site_t
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
  void parseS4(s4 value, int iBase = 0xA);       //4 byte signed
  void parseU4(u4 value, int iBase = 0xA);       //4 byte unsigned
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
  */
  int toInt() const;

  /*!
  Convert to signed 4 byte
  Follows strtol format if base is other than 10, signed 4 byte
  */
  s4 toS4(int iBase = 0xA) const;
  
  /*!
  Convert to unsigned 4 byte
  Follows strtoul format if base is other than 10, unsigned 4 byte
  */
  u4 toU4(int iBase = 0xA) const;
  
  /*!
  Convert to signed 8 byte
  Follows strtol format if base is other than 10, signed 4 byte
  */
  s8 toS8(int iBase = 0xA) const;

  /*!
  Convert to unsigned 8 byte
  Follows strtoul format if base is other than 10, unsigned 4 byte
  */
  u8 toU8(int iBase = 0xA) const;

  /*!
  Convert to unsigned size_t type
  Follows strtoul format if base is other than 10, unsigned 4 byte
  */
  size_t toSize_t(int iBase = 0xA) const;

  /*
  Convert to pointer
  Assumes 0xXXXXXXXX and returns pointer to it
  */
  void *toPointer() const;

  /*!
  Conversion from UTF-8 to Unicode
  Will get the first UTF-8 sequence and convert to Unicode wide-char
  */
  wchar_t toUnicode() const;

  /*!
  Conversion from UTF-8 to Unicode
  Will get the first UTF-8 sequence and convert to Unicode wide-char and return next position
  NOTE: When uStartPos is passed in, the return is the new position
        If the uStartPos == AConstant::npos, it is the end of the sequence (also denoted by a 0x0 return)
        Return of 0x0 and uStartPos != AConstant::npos signifies conversion is invalid (ie. invalid character at that pos, et al)
  */
  wchar_t toUnicode(size_t& startPos) const;

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
  Difference
   returns the first position after index at which the strings are different
   if identical npos is returned
  */
  size_t diff(const AString& strSource, size_t startIndex = 0) const;

  /*!
  Assignment
  */
  void assign(char cSource)                                                    { _assign(&cSource, 1); }
  void assign(const char *pccSource, size_t length = AConstant::npos)          { _assign(pccSource, length); }
  void assign(const unsigned char *pccSource, size_t length = AConstant::npos) { _assign((const char *)pccSource, length); }
  void assign(const AString& strSource, size_t length = AConstant::npos);
    
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
  Splits the string based on a delimeter character, empty strings discarded (i.e. 2 delimeters next to each other, leading delimeter, or trailing delimeter)
  strStrip will be used on each resulting string and stripped from front and back (AString::sstr_Empty means don't strip anything)
  bKeepEmpty if true it will keep empty string (2 delimeters next to each other)
  NOTE: Does not clear the list, appends
  */
  size_t split(LIST_AString& slResult, char delimeter = ' ', const AString& strStrip = AString::sstr_Empty, bool bKeepEmpty = false) const;
  size_t split(VECTOR_AString& slResult, char delimeter = ' ', const AString& strStrip = AString::sstr_Empty, bool bKeepEmpty = false) const;
  
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
  Cleaning methods
  */
  AString& stripLeading(const AString& strDelimeters = AString::sstr_WhiteSpace);
  AString& stripTrailing(const AString& strDelimeters = AString::sstr_WhiteSpace);
  AString& stripEntire(const AString& strDelimeters = AString::sstr_WhiteSpace);

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
  peekUntil(AString&,size_t,const AString&) will copy bytes from a given index until one of the delimeters is found or everything to end if not; returns new position or npos if not found
  */
  inline u1 peek(size_t index = 0) const;
  size_t peek(AOutputBuffer& bufDestination, size_t index = 0, size_t bytes = AConstant::npos) const;
  size_t peekUntil(AOutputBuffer& bufDestination, size_t index = 0, const AString& delimeters = AString::sstr_WhiteSpace) const;
  size_t peekUntil(AOutputBuffer& bufDestination, size_t sourceIndex, char delimeter) const;

  /*!
  Indexed access operators
    use(size_t) - will allow modification of the element at offset
  */
  inline u1& use(size_t index);

  /*!
  get functions (unlike peek) will get the content and remove it
  get(size_t) will return the byte at a given index
  get(AString&,size_t,size_t) will copy and remove bytes from a given index into another buffer
  getUntil(AString&,size_t,const AString&,bool) will copy and remove bytes until one of the delimeters is found;
     removeDelimeters flag means that the delimeter that was first found is removed ("Hello World" will return "Hello" and leave "World", if false " World" is left
     returns bytes copied or AConstant::npos if delimeter not found
  */
  inline u1 get(size_t index = 0);
  inline u1 rget();                                                                                      //a_Gets and removes the last byte
         size_t get(AString& bufDestination, size_t sourceIndex = 0, size_t bytes = AConstant::npos);              //a_Return # of bytes moved
         size_t getUntil(AString& bufDestination, const AString& delimeters = AString::sstr_WhiteSpace, bool removeDelimeters = true);     //a_Return # of bytes moved
         size_t getUntil(AString& bufDestination, char delimeter, bool removeDelimeter = true);                                            //a_Return # of bytes moved

  /*!
  remove - Pop removes count at a position
  rremove - Pops from the end, effectively shrinking the string
  */
  inline void remove(size_t length = 1, size_t offset = 0);
  inline void rremove(size_t length = 1);

  /*!
  Remove until a given delimeter
  */
  void removeUntil(const AString& delimeters = AString::sstr_WhiteSpace, bool removeDelimeters = true);
  void removeUntil(char delimeter, bool removeDelimeter = true);

  /*!
  set(u1,size_t) will set 1 byte at an index (yes, its really a poke! :)
  set(AString&,size_t) sets this buffer with source at an index and grows buffer as needed
  set(AString&,size_t,size_t,size_t) sets this buffer with source at an index and grows buffer as needed and int in length
  */
  inline void set(u1 uByte, size_t index);
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
  If requested the physical size will be returned which is to be used
   mostly for information purposes as it may have no bearing on logical buffer size
  */
  size_t getSize(bool boolPhysicalBuffer = false) const;

  /*!
  Sets the new physical size of the buffer
  Pads the current content with a given byte
  To only allocate the space use reserve() instead
  */
  void setSize(size_t newSize, u1 u1Pad = ' ');

  /*!
  Returns true if the buffer is logically empty
  */
  inline bool isEmpty() const;

  /*!
  Reserves additional space for the string only, does not alter content, just makes sure additional space is allocated
  Reserving more space doesn't mean you can access it, just allocated for future use (useful before doing lots of appends)
  To resize the actual string use setSize(...) call
  */
  void reserveMoreSpace(size_t moreSpace) { _resize(getSize() + moreSpace); }
  
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
  virtual void _append(const char *pccSource, size_t length);

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

public:
  //a_Default allocation block
  static const u2 smi_DefaultBufferIncrement;

//!!!ASerializable {
	virtual void toAFile(AFile& aFile) const;
  virtual void fromAFile(AFile& aFile);
//!!!ASerializable }

public:
  //a_Constants
  static const AString sstr_Empty;         //a_""  empty string
  static const AString sstr_Zero;          //a_"0" zero
  static const AString sstr_One;           //a_"1" one
  static const AString sstr_Two;           //a_"2" two
  static const AString sstr_Tab;           //a_"\t" one tab
  static const AString sstr_Space;         //a_" " one space
  static const AString sstr_TwoSpaces;     //a_"  " two spaces
  static const AString sstr_SingleQuote;   //a_' single quote
  static const AString sstr_DoubleQuote;   //a_" double quote
  static const AString sstr_WhiteSpace;    //a_" \r\n\t" whitespace
  static const AString sstr_CRLF;          //a_"\r\n" always
  static const AString sstr_CRLFCRLF;      //a_"\r\n\r\n" always
  static const AString sstr_CR;            //a_"\r" always
  static const AString sstr_LF;            //a_"\n" always
  static const AString sstr_EOL;           //a_"\r\n" on Windows, "\n" otherwise
  static const AString sstr_Null;          //a_"(null)" something to denote null value
  static const AString sstr_Colon;         //a_":" colon
  static const AString sstr_Semicolon;     //a_";" semicolon
  static const AString sstr_Period;        //a_"." period
  static const AString sstr_Comma;         //a_"," comma
  static const AString sstr_LessThan;      //a_"<"
  static const AString sstr_GreaterThan;   //a_">"
  static const AString sstr_OpenBrace;     //a_"{"
  static const AString sstr_CloseBrace;    //a_"}"
  static const AString sstr_Equals;        //a_"="
  static const AString sstr_Slash;         //a_"/"
  static const AString sstr_DoubleSlash;    //a_"//"
  static const AString sstr_True;          //a_"true"
  static const AString sstr_False;         //a_"false"

  static const AString sstr_UrlSafeAlphaNum;      //a_"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=_"
  static const AString sstr_AlphaNum;             //a_"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
  static const AString sstr_UpperCaseLetters;     //a_"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  static const AString sstr_LowerCaseLetters;     //a_"abcdefghijklmnopqrstuvwxyz"
  static const AString sstr_Numbers;              //a_"0123456789"
  static const AString sstr_LowerCaseConsonants;  //a_"bcdfghjklmnpqrstvwxyz"
  static const AString sstr_LowerCaseVowels;      //a_"aeiou"

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AString_HPP__

