#include "pchABase.hpp"
#include "AException.hpp"
#include "AXmlElement.hpp"
#include <errno.h>

AException::AException
(
  const ADebugDumpable *const pObject, // = NULL
  int iWhat,                           // = AException::Unknown
  const char* pccFilename,             // = ""
  int iLineNumber,                     // = 0
  const AString& strExtra,             // = AString::sstr_Empty
  errno_t errornum,                    // = 0
  bool walkStack                       // = false
) :
  m_ID(iWhat), 
  mp_Filename(pccFilename), 
	m_LineNumber(iLineNumber),
  m_ExtraText(strExtra),
  m_StackWalker(AStackWalker::SWO_SET_STACKONLY),
  m_errno(errornum)
{
#ifdef __DEBUG_DUMP__
  if (pObject)
  {
    pObject->debugDumpToAOutputBuffer(m_DebugDump);
  }
#endif
  
  //a_Get stack trace
  if (walkStack)
    m_StackWalker.ProcessCallstack();
}

AException::AException(const AException &that) :
  m_ID(that.m_ID),
  mp_Filename(that.mp_Filename),
  m_LineNumber(that.m_LineNumber),
  m_errno(that.m_errno),
  m_ExtraText(that.m_ExtraText),
  m_StackWalker(that.m_StackWalker),
  m_DebugDump(that.m_DebugDump)
{
}

AException::~AException()
{
}

AException& AException::operator=(const AException &that)
{
  //a_Assignment to self
  if (this == &that) return *this;

  m_ID          = that.m_ID;
  mp_Filename    = that.mp_Filename;         //a_Using save data (only pointer copied)
  m_LineNumber  = that.m_LineNumber;
  m_errno       = that.m_errno;
  m_ExtraText   = that.m_ExtraText;
  m_StackWalker = that.m_StackWalker;
  
  return *this;
}

void AException::add(const AString& str)
{
  m_ExtraText += str;
}

void AException::getDescription(AOutputBuffer& target) const throw()
{
  switch(_getID())
  {
    case ProgrammingError:
      target.append("Programming error."); break;

    case SystemError:
      target.append("Operating system error."); break;

    case SocketError:
      target.append("Socket error."); break;

    case NotImplemented:
      target.append("Not implemented."); break;

    case InvalidParameter:
      target.append("Invalid parameter specified."); break;

    case InvalidReturnValue:
      target.append("Return value is invalid or unexpected."); break;

    case NotSupported:
      target.append("Functionality not supported."); break;

    case InvalidObject:
      target.append("Invalid object integrity."); break;

    case InvalidState:
      target.append("Invalid state."); break;

    case InvalidConfiguration:
      target.append("Invalid configuration."); break;

    case TypeidMismatch:
      target.append("Mismatch between RTTI typeid's occured, different class types detected."); break;

    case OperationFailed:
      target.append("Operation failed."); break;

    case DeserializationError:
      target.append("De-serialization failed."); break;

    case APIFailure:
      target.append("API returned an unexpected result."); break;

    case ObjectIsASingleton:
      target.append("Object is a singleton and cannot be instanciated twice."); break;

    case AssertionFailed:
      target.append("Assertion failed."); break;

    case VerificationFailed:
      target.append("Verification failed."); break;

    case InitializationFailure:
      target.append("Initialization failed."); break;

    case InvalidData:
      target.append("Invalid data."); break;

    case InvalidEncoding:
      target.append("Invalid encoding or character set used."); break;

    case MemberNotInitialized:
       target.append("Member was not set or initialized correctly."); break;

    case DataConflict:
      target.append("Data conflict."); break;

    case IndexOutOfBounds:
      target.append("Index out of bounds."); break;

    case DoesNotExist:
      target.append("Item requested does not exist."); break;

    case NegativeLength:
      target.append("Negative length specified."); break;

    case InputSourceInvalid:
      target.append("Input source is invalid."); break;

    case OutputSourceInvalid:
      target.append("Output source is invalid."); break;

    case InputBufferIsNull:
      target.append("Input buffer is NULL."); break;

    case OutputBufferIsNull:
      target.append("Output buffer is NULL."); break;

    case InvalidLength:
      target.append("Invalid length."); break;

    case UnableToOpen:
      target.append("Cannot open."); break;

    case UnableToRemove:
      target.append("Cannot remove."); break;

		case UnableToMove:
      target.append("Cannot move."); break;

		case UnableToRead:
      target.append("Cannot read."); break;

		case UnableToWrite:
      target.append("Cannot write."); break;

		case UnableToFlush:
      target.append("Cannot flush."); break;

    case NotOpen:
      target.append("Operation cannot be performed on an unopened file type."); break;

    case EmptyFilename:
      target.append("Filename cannot be empty for this operation."); break;

    case InsuffiecientData:
      target.append("Insuffiecient data, buffer underrun."); break;

    case UnknownType :
      target.append("Name-Value pair type is unknown or not set."); break;

    case EndOfBuffer :
      target.append("Encountered an end-of-buffer while parsing NV pair."); break;

    case EmptyName :
      target.append("Name cannot be empty."); break;

    case ReadBeyondEndOfBuffer:
      target.append("Read beyond end of buffer, bytes requested is more that contained in the buffer."); break;

    case NotEnoughDataInSource:
      target.append("The source buffer did not contain enough data needed for a read."); break;

    case SourceBufferIsNull:
      target.append("Source buffer passed in is NULL."); break;

    case TooManyDecimalPoints:
      target.append("Detected more that one decimal points."); break;

    case TooManyExponentSpecifiers:
      target.append("Detected more that one exponent specifiers."); break;

    case InfinityUnexpected:
      target.append("Infinity is not a valid value for this operation."); break;

    case InfinityOverInfinity:
      target.append("Infinity / Infinity is undefined."); break;

    case ZeroOverZero:
      target.append("Zero / Zero is undefined."); break;

    case ZeroOverInfinity:
      target.append("Zero / Infinity is undefined."); break;

    case InfinityTimesZero:
      target.append("Infinity * Zero is undefined."); break;

    case ZeroTimesInfinity:
      target.append("Zero * Infinity is undefined."); break;

    case InfinityOverZero:
      target.append("Infinity / Zero is undefined."); break;

		case NegativePrecision:
      target.append("Negative precision is invalid."); break;

    case NANUnexpected:
      target.append("NAN not expected for this operation."); break;
  
    case InvalidCharacter:
      target.append("Invalid character detected in the number."); break;

    case NegativeFactorial:
      target.append("Factorial for negative numbers is undefined."); break;

    case NonWholeFactorial:
      target.append("Factorial for non-whole numbers is undefined."); break;

    case NegativeSquareRoot:
      target.append("Square root of a negative number is imaginary."); break;

    case ObjectContainerCollision:
      target.append("Object collided with existing component of the container."); break;

    case CannotInsertUnnamedObject:
      target.append("Unable to insert an unnamed object into a container."); break;

    case InvalidProtocol:
      target.append("Protocol specified is not valid."); break;
  
    case InvalidPathRedirection:
      target.append("Path redirection via '..' going below root."); break;

    case InvalidToken:
      target.append("HTTP token is invalid."); break;

    case RequestInvalidMethod:
      target.append("Invalid request method."); break;

    case RequestInvalidVersion:
      target.append("Invalid HTTP version in request."); break;

    case ResponseInvalidVersion:
      target.append("Invalid HTTP version in response."); break;

    case InvalidPath:
      target.append("Invalid path."); break;

    case Unknown :
    default :
      target.append("Unknown."); break;
  }
}

const ARope& AException::what() const throw()
{
  //a_Already generated
  if (!mm_What.isEmpty())
    return mm_What;

  getDescription(mm_What);
  if (!m_ExtraText.isEmpty())
  {
    mm_What.append('(');
    mm_What.append(m_ExtraText);
    mm_What.append(')');
  }
  mm_What.append(" @ ",3);
  mm_What.append(mp_Filename);
  mm_What.append(':');
  mm_What.append(AString::fromInt(m_LineNumber));
  if (m_errno)
  {
    mm_What.append("  errno=",8);
    mm_What.append(AString::fromInt(m_errno));
    mm_What.append(" (",2);
    getErrnoMessage(mm_What, m_errno);
    mm_What.append(')');
  }
  mm_What.append(AString::sstr_EOL);

  m_StackWalker.emit(mm_What);

  if (!m_DebugDump.isEmpty())
  {
    mm_What.append(AString::sstr_EOL);
    mm_What.append(m_DebugDump);
  }
  mm_What.append(AString::sstr_EOL);

  return mm_What;
}

void AException::emit(AOutputBuffer& target) const
{
  what().emit(target);
}

void AException::emit(AXmlElement& target) const
{
  if (target.useName().isEmpty())
    target.useName().assign(ASW("AException",10));
  
  ARope rope;
  getDescription(rope);
  target.addElement(ASW("desc",4), rope, AXmlData::CDataSafe);
  target.addElement(ASW("id",2), (u4)m_ID);
  target.addElement(ASW("filename",8), ASWNL(mp_Filename), AXmlData::CDataDirect);
  target.addElement(ASW("linenum",7), (u4)m_LineNumber);
  target.addElement(ASW("extra",5), m_ExtraText, AXmlData::CDataSafe);
  target.addElement(ASW("errno",5), (u4)m_errno);
  target.addElement(ASW("stacktrace",10), m_StackWalker);
  target.addElement(ASW("debugdump",9), m_DebugDump);
}

const char* AException::getFilename() const throw()
{ 
  return mp_Filename;
}

int AException::getLineNumber() const throw()
{ 
  return m_LineNumber; 
}

void AException::getErrnoMessage(AOutputBuffer& target, errno_t errornum)
{
  switch(errornum)
  {
    case EPERM:
      target.append("Operation not permitted");
      break;
 
    case ENOENT:
      target.append("No such file or directory");
      break;
 
    case ESRCH:
      target.append("No such process");
      break;
 
    case EINTR:
      target.append("Interrupted function");
      break;
 
    case EIO:
      target.append("I/O error");
      break;

    case ENXIO:
      target.append("No such device or address");
      break;

    case E2BIG:
      target.append("Argument list too long");
      break;

    case ENOEXEC:
      target.append("Exec format error");
      break;

    case EBADF:
      target.append("Bad file number");
      break;

    case ECHILD:
      target.append("No spawned processes");
      break;

    case EAGAIN:
      target.append("No more processes or not enough memory or maximum nesting level reached");
      break;

    case ENOMEM:
      target.append("Not enough memory");
      break;

    case EACCES:
      target.append("Permission denied");
      break;

    case EFAULT:
      target.append("Bad address");
      break;

    case EBUSY:
      target.append("Device or resource busy");
      break;

    case EEXIST:
      target.append("File exists");
      break;

    case EXDEV:
      target.append("Cross-device link");
      break;

    case ENODEV:
      target.append("No such device");
      break;

    case ENOTDIR:
      target.append("Not a directory");
      break;

    case EISDIR:
      target.append("Is a directory");
      break;

    case EINVAL:
      target.append("Invalid argument");
      break;

    case ENFILE:
      target.append("Too many files open in system");
      break;

    case EMFILE:
      target.append("Too many open files");
      break;

    case ENOTTY:
      target.append("Inappropriate I/O control operation");
      break;

    case EFBIG:
      target.append("File too large");
      break;

    case ENOSPC:
      target.append("No space left on device");
      break;

    case ESPIPE:
      target.append("Invalid seek");
      break;

    case EROFS:
      target.append("Read-only file system");
      break;

    case EMLINK:
      target.append("Too many links");
      break;

    case EPIPE:
      target.append("Broken pipe");
      break;

    case EDOM:
      target.append("Math argument");
      break;

    case ERANGE:
      target.append("Result too large");
      break;

    case EDEADLK:
      target.append("Resource deadlock would occur");
      break;

    case ENAMETOOLONG:
      target.append("Filename too long");
      break;

    case ENOLCK:
      target.append("No locks available");
      break;

    case ENOSYS:
      target.append("Function not supported");
      break;

    case ENOTEMPTY:
      target.append("Directory not empty");
      break;

    case EILSEQ:
      target.append("Illegal byte sequence");
      break;

#if (_MSC_VER >= 1400)
    case STRUNCATE:
      target.append("String was truncated");
      break;
#endif

    default:
      target.append("none");
      break;
  }
}

errno_t AException::getErrno() const throw()
{ 
  return m_errno; 
}

const AStackWalker& AException::getStackWalker() const
{
  return m_StackWalker;
}
