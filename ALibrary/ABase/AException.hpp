#ifndef INCLUDED__AException_HPP__
#define INCLUDED__AException_HPP__

#include "apiABase.hpp"
#include "AEmittable.hpp"
#include "AXmlEmittable.hpp"
#include "AString.hpp"
#include "ARope.hpp"
#include "AStackWalker.hpp"
#include <exception>
#include <cassert>

/*!
This is the base exception class.
There are 2 important issues for creating your own exception:
  1. Use unique ID enums
  2. getDescription() must return value for your class and by
     default return the value of the direct parent
*/
class ABASE_API AException : public AEmittable, public AXmlEmittable
{
  public:
    /*!
     This exists so that each derived exception provides a list
     of exception types and the actual message can be made
     language-independent
     generic exception mask: 0x000000XX
    */
    enum
    {
      // Generic
      Unknown                    = 0x00000000,  //a_Exception either uninitialized or unknown
      ProgrammingError           = 0x00000001,  //a_Usually thrown when unexpected behavior is encountered
      NotImplemented             = 0x00000002,  //a_Not yet implemented
      InvalidParameter           = 0x00000003,  //a_Invalid parameter specified when calling a method
      InvalidReturnValue         = 0x00000004,  //a_Return value is invalid or unexpected
      NotSupported               = 0x00000005,  //a_Functionality not supported
      InvalidObject              = 0x00000006,  //a_Object is detected to be invalid
      TypeidMismatch             = 0x00000007,  //a_RTTI typeid mismatch occured
      OperationFailed            = 0x00000008,  //a_Some operation failed
      DeserializationError       = 0x00000009,  //a_Unable to deserialize correctly
      APIFailure                 = 0x0000000A,  //a_API failure
      ObjectIsASingleton         = 0x0000000B,  //a_Object is a singleton and cannot be instanciated twice
      AssertionFailed            = 0x0000000C,  //a_Assertion failed
      VerificationFailed         = 0x0000000D,  //a_Verification failed
      InvalidData                = 0x0000000E,  //a_Data is invalid
      MemberNotInitialized       = 0x0000000F,  //a_Class member was not set/initilized correctly
      DataConflict               = 0x00000010,  //a_Data generated a conflict
      InvalidState               = 0x00000011,  //a_Invalid state
      InvalidEncoding            = 0x00000012,  //a_Invalid encoding or character set used
      InvalidConfiguration       = 0x00000013,  //a_Invalid configuration
      InitializationFailure      = 0x00000014,  //a_Failed to initialize
      NotFound                   = 0x00000015,  //a_Not found

      // String based
      ReadBeyondEndOfBuffer      = 0x00000100,  //a_Reading a buffer beyond the end
      SourceBufferIsNull         = 0x00000101,  //a_Source buffer passed in is NULL
      NotEnoughDataInSource      = 0x00000102,  //a_The source buffer did not contain enough data needed for a read

      // File based
      NegativeLength             = 0x00000200,  //a_Length used is <0
      InputSourceInvalid         = 0x00000201,  //a_Input source is invalid
      OutputSourceInvalid        = 0x00000202,  //a_Output source is invalid
      InputBufferIsNull          = 0x00000203,  //a_Input buffer is NULL
      OutputBufferIsNull         = 0x00000204,  //a_Output buffer is NULL
      InvalidLength              = 0x00000205,  //a_Invalid buffer length
      InvalidBinaryLength        = 0x00000206,  //a_Binary quantity of invalid length (not 1,2,4,8)
      NotOpen                    = 0x00000207,  //a_File type was not opened
      EmptyFilename              = 0x00000208,  //a_Filename specified is empty
      UnableToOpen               = 0x00000209,  //a_Unable to open a file
			UnableToRemove             = 0x00000210,  //a_Unable to remove
			UnableToMove               = 0x00000211,  //a_Unable to move
      InsuffiecientData          = 0x00000212,  //a_Buffer read underflow
      UnableToRead               = 0x00000213,  //a_Unable to read
      UnableToWrite              = 0x00000214,  //a_Unable to write
      UnableToFlush              = 0x00000215,  //a_Unable to flush

      // Number based
      NegativePrecision          = 0x00000300, //a_Negative precision is invalid
      TooManyDecimalPoints       = 0x00000301, //a_Found more that one decimal points
      TooManyExponentSpecifiers  = 0x00000302, //a_Found more that one exponent specifiers
      InfinityUnexpected         = 0x00000303, //a_Infinity not expected
      InfinityOverInfinity       = 0x00000304, //a_Infinity / Infinity is not defined
      ZeroOverZero               = 0x00000305, //a_Zero / Zero is undefined
      ZeroOverInfinity           = 0x00000306, //a_Zero / Infinity is not defined
      InfinityOverZero           = 0x00000307, //a_Infinity / Zero is undefined
      ZeroTimesInfinity          = 0x00000308, //a_Zero * Infinity is not defined
      InfinityTimesZero          = 0x00000309, //a_Infinity * Zero is undefined
      NANUnexpected              = 0x00000310, //a_NAN not expected
      InvalidCharacter           = 0x00000311, //a_Invalid characted detected in the number
      NegativeFactorial          = 0x00000312, //a_Factorial for negative numbers is undefined
      NonWholeFactorial          = 0x00000313, //a_Factorial for non-whole numbers is undefined
      NegativeSquareRoot         = 0x00000314, //a_Square root of a negative number (complex not supported)

      // NameValue pair based
      UnknownType                = 0x00000400,  //a_NV pair type was not set or is invald
      EndOfBuffer                = 0x00000401,  //a_End of buffer while parsing
      EmptyName                  = 0x00000402,  //a_Name cannot be empty

      // Container based
      IndexOutOfBounds           = 0x00000500,  //a_Index is <0 or >length()
      DoesNotExist               = 0x00000501,  //a_Item does not exist in the container

      // Object model based
      ObjectContainerCollision   = 0x00000600,  //a_Collision during insert with container component (like path part)
      CannotInsertUnnamedObject  = 0x00000601,  //a_Inserted object must have a name

      //a_URL related
      InvalidProtocol            = 0x00004000,  //a_Invalid protocol type or format
      InvalidPathRedirection     = 0x00004001,  //a_Invalid path redirection (using '..' beyond root)
      InvalidPath                = 0x00004002,  //a_Trying to access an invalid path

      //a_HTTP header related
      InvalidToken               = 0x00004100,  //a_Invalid protocol type or format
      RequestInvalidMethod       = 0x00004120,  //a_Request is not valid
      RequestInvalidVersion      = 0x00004121,  //a_Request version of HTTP is invalid
      ResponseInvalidVersion     = 0x00004140,  //a_Response version invalid

      //a_System error
      SystemError                = 0x00010000,  //a_System error occured, subclass is handling it
      SocketError                = 0x00010001   //a_System error occured, subclass is handling it
    };

  public:
	  /*!
    Default construction is of an unknown exception
    */
#ifdef ENABLE_EXCEPTION_STACKWALK
    AException(
      const ADebugDumpable *const pObject = NULL,
      int iID = AException::Unknown, 
      const char* pccFilename = "", 
      int iLineNumber = 0, 
      const AEmittable& extra = AConstant::ASTRING_EMPTY,
      errno_t errornum = 0,
      bool walkStack = true
    );
#else
    AException(
      const ADebugDumpable *const pObject = NULL,
      int iID = AException::Unknown, 
      const char* pccFilename = "", 
      int iLineNumber = 0, 
      const AEmittable& extra = AConstant::ASTRING_EMPTY,
      errno_t errornum = 0,
      bool walkStack = false
    );
#endif

    /*!
    Copy ctor
    */
    AException(const AException &that);

    /*!
    Copy operator
    */
    AException& operator=(const AException &that);

    /*!
    Emit exception
    */
    virtual void emit(AOutputBuffer& target) const;
    virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

    /*!
    General method for retrieving the exception description and location
    NOTE: Do not override this method, getDescription() is what you override
    */
    const ARope& what() const throw();

    /**
     * Adds to the description of the exception, useful when catching and rethrowing
    **/
    void add(const AString&);

    /*!
    dtor
    */
	  virtual ~AException();

	  /*!
    Access to the components of this exception
    */
    const char* getFilename() const throw();
  	int getLineNumber() const throw();
    errno_t getErrno() const throw();

    /*!
    Gets the description of an exception type
     and by default should return the description
     of the parent class.
    All derived exceptions must follow this pattern to function in unison.
    */
    virtual void getDescription(AOutputBuffer&) const throw();

    /*!
    Stack walker
    */
    const AStackWalker& getStackWalker() const;

    /*!
    errno utility
    */
    static void getErrnoMessage(AOutputBuffer& target, errno_t errornum);

  protected:
    /*!
    Internal methods to be used by child classes
    */
    int  _getID() const  { return m_ID; }
    void _setID(int iID) { m_ID = iID; }

  	int            m_ID;
  	const char*    mp_Filename;
    int            m_LineNumber;
    errno_t        m_errno;
    AString        m_ExtraText;
    AStackWalker   m_StackWalker;
    ARope          m_DebugDump;

  private:
    /*!
    This is a transient class that is used to hold the content of the exception
    */
    mutable ARope mm_What;
};

/*!
Necessary macro for throwing diffent types of exceptions
 one must use this macro as _FILE_ and _LINE_ are set
 when the macro is expanded and if these were generalized
 as a default parameter then all exceptions would point to
 the AException.hpp to the line of the constructor.
*/
#define ATHROW(debugdumpable, _exceptionID) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__)

#define ATHROW_EX(debugdumpable, _exceptionID, _exceptionExtraText) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, _exceptionExtraText)

#define ATHROW_ERRNO(debugdumpable, _exceptionID, errornum) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, AConstant::ASTRING_EMPTY, errornum)

#define ATHROW_ERRNO_EX(debugdumpable, _exceptionID, _exceptionExtraText, errornum) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, _exceptionExtraText, errornum)

/*!
AASERT_xxx() will throw an exception if debugging is turned on and if the condition is true
Otherwise it will do nothing
*/
#if defined(_DEBUG)
//#define AASSERT(this, condition) assert(condition)
//#define AASSERT_EX(this, condition,msg) assert(condition && msg)
#define AASSERT(debugdumpable, condition)          if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }
#define AASSERT_EX(debugdumpable, condition, msg)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, msg); }
#define AVERIFY(debugdumpable, condition)          if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }
#define AVERIFY_EX(debugdumpable, condition, msg)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, msg); }
#elif defined(NDEBUG)
#define AASSERT(debugdumpable, condition)          ((void *)0x0)
#define AASSERT_EX(debugdumpable, condition, msg)  ((void *)0x0)
#define AVERIFY(debugdumpable, condition)          if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }
#define AVERIFY_EX(debugdumpable, condition, msg)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, msg); }
#else
#error Must define either _DEBUG (debug mode, assertions ON) or NDEBUG (release mode, no assertions)
#endif

#endif // INCLUDED__AException_HPP__
