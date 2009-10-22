/*
Written by Alex Chachanashvili

$Id$
*/
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
This is the base exception class

Best used through ATHROW_XXX and AASSERT macros at the bottom of this header

There are 2 important issues for creating your own exception:
  1. Use unique ID enums
  2. getDescription() must return value for your class and by
     default return the value of the direct parent

@see ATHROW
@see ATHROW_EX
@see ATHROW_ERRNO
@see ATHROW_ERRNO_EX
@see ATHROW_
@see ATHROW_LAST_SOCKET_ERROR
@see ATHROW_LAST_SOCKET_ERROR_KNOWN
@see ATHROW_LAST_SOCKET_ERROR_KNOWN_EX
@see ATHROW_LAST_OS_ERROR
@see ATHROW_LAST_OS_ERROR_EX
@see ATHROW_LAST_OS_ERROR_KNOWN
@see ATHROW_LAST_OS_ERROR_KNOWN_EX
@see AASSERT
@see AASSERT_EX
@see AVERIFY
@see AVERIFY_EX
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
    Unknown                    = 0x00000000,  //!< Exception either uninitialized or unknown
    ProgrammingError           = 0x00000001,  //!< Usually thrown when unexpected behavior is encountered
    NotImplemented             = 0x00000002,  //!< Not yet implemented
    InvalidParameter           = 0x00000003,  //!< Invalid parameter specified when calling a method
    InvalidReturnValue         = 0x00000004,  //!< Return value is invalid or unexpected
    NotSupported               = 0x00000005,  //!< Functionality not supported
    InvalidObject              = 0x00000006,  //!< Object is detected to be invalid
    TypeidMismatch             = 0x00000007,  //!< RTTI typeid mismatch occured
    OperationFailed            = 0x00000008,  //!< Some operation failed
    DeserializationError       = 0x00000009,  //!< Unable to deserialize correctly
    APIFailure                 = 0x0000000A,  //!< API failure
    ObjectIsASingleton         = 0x0000000B,  //!< Object is a singleton and cannot be instanciated twice
    AssertionFailed            = 0x0000000C,  //!< Assertion failed
    VerificationFailed         = 0x0000000D,  //!< Verification failed
    InvalidData                = 0x0000000E,  //!< Data is invalid
    MemberNotInitialized       = 0x0000000F,  //!< Class member was not set/initilized correctly
    DataConflict               = 0x00000010,  //!< Data generated a conflict
    InvalidState               = 0x00000011,  //!< Invalid state
    InvalidEncoding            = 0x00000012,  //!< Invalid encoding or character set used
    InvalidConfiguration       = 0x00000013,  //!< Invalid configuration
    InitializationFailure      = 0x00000014,  //!< Failed to initialize
    NotFound                   = 0x00000015,  //!< Not found

    // String based
    ReadBeyondEndOfBuffer      = 0x00000100,  //!< Reading a buffer beyond the end
    SourceBufferIsNull         = 0x00000101,  //!< Source buffer passed in is NULL
    NotEnoughDataInSource      = 0x00000102,  //!< The source buffer did not contain enough data needed for a read

    // File based
    NegativeLength             = 0x00000200,  //!< Length used is <0
    InputSourceInvalid         = 0x00000201,  //!< Input source is invalid
    OutputSourceInvalid        = 0x00000202,  //!< Output source is invalid
    InputBufferIsNull          = 0x00000203,  //!< Input buffer is NULL
    OutputBufferIsNull         = 0x00000204,  //!< Output buffer is NULL
    InvalidLength              = 0x00000205,  //!< Invalid buffer length
    InvalidBinaryLength        = 0x00000206,  //!< Binary quantity of invalid length (not 1,2,4,8)
    NotOpen                    = 0x00000207,  //!< File type was not opened
    EmptyFilename              = 0x00000208,  //!< Filename specified is empty
    UnableToOpen               = 0x00000209,  //!< Unable to open a file
		UnableToRemove             = 0x00000210,  //!< Unable to remove
		UnableToMove               = 0x00000211,  //!< Unable to move
    InsuffiecientData          = 0x00000212,  //!< Buffer read underflow
    UnableToRead               = 0x00000213,  //!< Unable to read
    UnableToWrite              = 0x00000214,  //!< Unable to write
    UnableToFlush              = 0x00000215,  //!< Unable to flush

    // Number based
    NegativePrecision          = 0x00000300, //!< Negative precision is invalid
    TooManyDecimalPoints       = 0x00000301, //!< Found more that one decimal points
    TooManyExponentSpecifiers  = 0x00000302, //!< Found more that one exponent specifiers
    InfinityUnexpected         = 0x00000303, //!< Infinity not expected
    InfinityOverInfinity       = 0x00000304, //!< Infinity / Infinity is not defined
    ZeroOverZero               = 0x00000305, //!< Zero / Zero is undefined
    ZeroOverInfinity           = 0x00000306, //!< Zero / Infinity is not defined
    InfinityOverZero           = 0x00000307, //!< Infinity / Zero is undefined
    ZeroTimesInfinity          = 0x00000308, //!< Zero * Infinity is not defined
    InfinityTimesZero          = 0x00000309, //!< Infinity * Zero is undefined
    NANUnexpected              = 0x00000310, //!< NAN not expected
    InvalidCharacter           = 0x00000311, //!< Invalid characted detected in the number
    NegativeFactorial          = 0x00000312, //!< Factorial for negative numbers is undefined
    NonWholeFactorial          = 0x00000313, //!< Factorial for non-whole numbers is undefined
    NegativeSquareRoot         = 0x00000314, //!< Square root of a negative number (complex not supported)

    // NameValue pair based
    UnknownType                = 0x00000400,  //!< NV pair type was not set or is invald
    EndOfBuffer                = 0x00000401,  //!< End of buffer while parsing
    EmptyName                  = 0x00000402,  //!< Name cannot be empty

    // Container based
    IndexOutOfBounds           = 0x00000500,  //!< Index is <0 or >length()
    DoesNotExist               = 0x00000501,  //!< Item does not exist in the container

    // Object model based
    ObjectContainerCollision   = 0x00000600,  //!< Collision during insert with container component (like path part)
    CannotInsertUnnamedObject  = 0x00000601,  //!< Inserted object must have a name

    // URL related
    InvalidProtocol            = 0x00004000,  //!< Invalid protocol type or format
    InvalidPathRedirection     = 0x00004001,  //!< Invalid path redirection (using '..' beyond root)
    InvalidPath                = 0x00004002,  //!< Trying to access an invalid path

    // HTTP header related
    InvalidToken               = 0x00004100,  //!< Invalid protocol type or format
    RequestInvalidMethod       = 0x00004120,  //!< Request is not valid
    RequestInvalidVersion      = 0x00004121,  //!< Request version of HTTP is invalid
    ResponseInvalidVersion     = 0x00004140,  //!< Response version invalid

    // System error
    SystemError                = 0x00010000,  //!< System error occured, subclass is handling it
    SocketError                = 0x00010001   //!< System error occured, subclass is handling it
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
    bool walkStack = false  //TODO fix
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

  //! copy ctor
  AException(const AException &);

  //! copy operator
  AException& operator=(const AException &);

  /*!
  AEmittable

  @param target to append to
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  AXmlEmittable

  @param thisRoot to append XML elements to
  @return thisRoot for convenience
  */
  virtual AXmlElement& emitXml(AXmlElement& thisRoot) const;

  /*!
  General method for retrieving the exception description and location
  NOTE: Do not override this method, getDescription() is what you override

  @return description of this exception
  @throw never
  */
  const ARope& what() const throw();

  /*!
  Adds to the description of the exception, useful when catching and rethrowing

  @param message to add to the current description
  */
  void add(const AString& message);

  //! virtual dtor
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

  @param target appended the description
  */
  virtual void getDescription(AOutputBuffer&) const throw();

  /*!
  Stack walker
  */
#if defined(__WINDOWS__) && defined(_DEBUG)
  const AStackWalker& getStackWalker() const;
#endif

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
  ARope          m_DebugDump;
#if defined(__WINDOWS__) && defined(_DEBUG)
  AStackWalker   m_StackWalker;
#endif

private:
  /*!
  This is a transient class that is used to hold the content of the exception
  */
  mutable ARope mm_What;
};

/*!
Necessary macro for throwing diffent types of exceptions one must use this macro as _FILE_ and _LINE_ are set
 when the macro is expanded and if these were generalized as a default parameter then all exceptions would point to
 the AException.hpp to the line of the constructor.

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param _exceptionID AException::XXX enum
@throw AException always
*/
#define ATHROW(debugdumpable, _exceptionID) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__)

/*!
Necessary macro for throwing diffent types of exceptions one must use this macro as _FILE_ and _LINE_ are set
 when the macro is expanded and if these were generalized as a default parameter then all exceptions would point to
 the AException.hpp to the line of the constructor.

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param _exceptionID AException::XXX enum
@param _exceptionExtraEmittableText AEmittable type that contains extra info
@throw AException always
*/
#define ATHROW_EX(debugdumpable, _exceptionID, _exceptionExtraEmittableText) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, _exceptionExtraEmittableText)

/*!
Necessary macro for throwing diffent types of exceptions one must use this macro as _FILE_ and _LINE_ are set
 when the macro is expanded and if these were generalized as a default parameter then all exceptions would point to
 the AException.hpp to the line of the constructor.

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param _exceptionID AException::XXX enum
@param errornum errno.h based error number
@throw AException always
*/
#define ATHROW_ERRNO(debugdumpable, _exceptionID, errornum) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, AConstant::ASTRING_EMPTY, errornum)

/*!
Necessary macro for throwing diffent types of exceptions one must use this macro as _FILE_ and _LINE_ are set
 when the macro is expanded and if these were generalized as a default parameter then all exceptions would point to
 the AException.hpp to the line of the constructor.

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param _exceptionID AException::XXX enum
@param _exceptionExtraEmittableText AEmittable type that contains extra info
@param errornum errno.h based error number
@throw AException always
*/
#define ATHROW_ERRNO_EX(debugdumpable, _exceptionID, _exceptionExtraEmittableText, errornum) \
  throw AException(debugdumpable, _exceptionID, __FILE__, __LINE__, _exceptionExtraEmittableText, errornum)

/*!
AASERT_xxx() will throw an exception if debugging is turned on and if the condition is true
Otherwise it will do nothing
*/

#if defined(_DEBUG)
/*!
Assert macro that throws an exception when condition fails
Does nothing if compiled in release mode

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param condition to test, if false exception will be thrown
@throw AException when condition is false, never if non-debug
*/
#define AASSERT(debugdumpable, condition)          if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }

/*!
Assert macro that throws an exception when condition fails
Does nothing if compiled in release mode

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param condition to test, if false exception will be thrown
@param message that gets added to exception
@throw AException when condition is false, never if non-debug
*/
#define AASSERT_EX(debugdumpable, condition, message)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, message); }

/*!
Verify macro that throws an exception when condition fails
Does nothing if compiled in release mode

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param condition to test, if false exception will be thrown
@throw AException when condition is false, never if non-debug
*/
#define AVERIFY(debugdumpable, condition)              if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }

/*!
Verify macro that throws an exception when condition fails
Does nothing if compiled in release mode

@param debugdumpable ADebugDumpable * to include in exception (calls debugDump())
@param condition to test, if false exception will be thrown
@param message that gets added to exception
@throw AException when condition is false, never if non-debug
*/
#define AVERIFY_EX(debugdumpable, condition, message)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, message); }
#elif defined(NDEBUG)
#define AASSERT(debugdumpable, condition)              ((void *)0x0)
#define AASSERT_EX(debugdumpable, condition, message)  ((void *)0x0)
#define AVERIFY(debugdumpable, condition)              if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__); }
#define AVERIFY_EX(debugdumpable, condition, message)  if (!(condition)) { throw AException(debugdumpable, AException::AssertionFailed, __FILE__, __LINE__, message); }
#else
#error Must define either _DEBUG (debug mode, assertions ON) or NDEBUG (release mode, no assertions)
#endif

#endif // INCLUDED__AException_HPP__
