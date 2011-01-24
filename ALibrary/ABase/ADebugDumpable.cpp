/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "ADebugDumpable.hpp"
#include "AOutputBuffer.hpp"
#include <sstream>

void ADebugDumpable::debugDump(std::ostream& os /* = std::cerr */, int indent /* = 0 */) const
{
  //a_By default display the type of the class and this pointer address
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
}

void ADebugDumpable::debugDumpToAOutputBuffer(AOutputBuffer& target, int indent /* = 0 */) const
{
  std::stringstream strm;
  debugDump(strm, indent);
  target.append(strm.str().c_str(), strm.str().size());
}

void ADebugDumpable::emit(AOutputBuffer& target) const
{
  debugDumpToAOutputBuffer(target, 0);
}

void ADebugDumpable::trace(const char *pcc)
{
  #ifdef __WINDOWS__
    OutputDebugString(pcc);
  #else
    std::cerr << pcc << std::flush;
  #endif
}

void ADebugDumpable::dumpMemory_Ascii(
  AOutputBuffer& target,
  const void *pvObject,
  size_t objectSize,
  size_t indent,      // = 0
  size_t bytesPerRow  // = 32
)
{
  char cX;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    //a_ASCII
    ADebugDumpable::indent(target, indent);
    for (x = 0; x < bytesPerRow; x++)
    {
      if (base * bytesPerRow + x >= objectSize)
        break;

      cX = *((char *)pvObject + base * bytesPerRow + x);
      if (AConstant::npos == AConstant::CHARSET_ALPHANUM64.find(cX))
        target.append('.');
      else
        target.append(cX);
    }
    target.append(AConstant::ASTRING_EOL);

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

void ADebugDumpable::dumpMemory_Ascii(
  std::ostream& os,
  const void *pvObject,
  size_t objectSize,
  size_t indent,      // = 0
  size_t bytesPerRow  // = 32
)
{
  char cX;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    //a_ASCII
    ADebugDumpable::indent(os, indent);
    for (x = 0; x < bytesPerRow; x++)
    {
      if (base * bytesPerRow + x >= objectSize)
        break;

      cX = *((char *)pvObject + base * bytesPerRow + x);
      if (u1(cX) < 0x20 || u1(cX) > 0x7F) //-V112
        os << '.';
      else
        os << cX;
    }
    os << std::endl;

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

void ADebugDumpable::dumpMemory_HexAscii(
  AOutputBuffer& target,
  const void *pvObject,
  size_t objectSize,
  size_t indent,       // = 0
  size_t bytesPerRow   // = 16
)
{
  u1 u1X;
  char cX;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    ADebugDumpable::indent(target, indent);

    //a_ASCII
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
        break;

      cX = *((char *)pvObject + base * bytesPerRow + x);
      if (AConstant::npos == AConstant::CHARSET_ALPHANUM64.find(cX))
        target.append('.');
      else
        target.append(cX);
    }
    for (;x < bytesPerRow;++x)   //a_Pad space after : to align hex
      target.append(' ');

    target.append(": ",2);

    //a_Hex
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
      {
        for (;x < bytesPerRow; ++x)
          target.append("   ",3);

        break;
      }

      u1X = *((unsigned char *)pvObject + base * bytesPerRow + x);
      target.append(u1X < 16 ? AConstant::ASTRING_ZERO : AConstant::ASTRING_EMPTY);
      target.append(AString::fromU1(u1X, 16));
      target.append(' ');
    }

    target.append(AConstant::ASTRING_EOL);

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

void ADebugDumpable::dumpMemory_HexAscii(
  std::ostream& os,
  const void *pvObject,
  size_t objectSize,
  size_t indent,       // = 0
  size_t bytesPerRow   // = 16
)
{
  u1 u1X;
  char cX;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    ADebugDumpable::indent(os, indent);

    //a_ASCII
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
        break;

      cX = *((char *)pvObject + base * bytesPerRow + x);
      if (u1(cX) < 0x20 || u1(cX) > 0x7F) //-V112
        os << '.';
      else
        os << cX;
    }
    for (;x < bytesPerRow;++x)   //a_Pad space after : to align hex
      os << " ";

    os << ": ";

    //a_Hex
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
      {
        for (;x < bytesPerRow; ++x)
          os << "   ";

        break;
      }

      u1X = *((unsigned char *)pvObject + base * bytesPerRow + x);
      os << (u1X < 16 ? "0" : "") << std::hex << (int)u1X << " ";
    }

    os << std::endl;

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

void ADebugDumpable::dumpMemory_Hex(
  AOutputBuffer& target,
  const void *pvObject,
  size_t objectSize,
  size_t indent,      // = 0
  size_t bytesPerRow  // = 16
)
{
  u1 u1X;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    //a_Hex
    ADebugDumpable::indent(target, indent);
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
      {
        for (;x < bytesPerRow; ++x)
          target.append("   ",3);

        break;
      }

      u1X = *((unsigned char *)pvObject + base * bytesPerRow + x);
      target.append(u1X < 16 ? AConstant::ASTRING_ZERO : AConstant::ASTRING_EMPTY);
      target.append(AString::fromU1(u1X, 16));
      target.append(' ');
    }
    target.append(AConstant::ASTRING_EOL);

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

void ADebugDumpable::dumpMemory_Hex(
  std::ostream& os,
  const void *pvObject,
  size_t objectSize,
  size_t indent,      // = 0
  size_t bytesPerRow  // = 16
)
{
  u1 u1X;
  size_t x;
  size_t base = 0;

  for(;;base++)
  {
    //a_Hex
    ADebugDumpable::indent(os, indent);
    for (x = 0; x < bytesPerRow; ++x)
    {
      if (base * bytesPerRow + x >= objectSize)
      {
        for (;x < bytesPerRow; ++x)
          os << "   ";

        break;
      }

      u1X = *((unsigned char *)pvObject + base * bytesPerRow + x);
      os << (u1X < 16 ? "0" : "") << std::hex << (int)u1X << " ";
    }
    os << std::endl;

    if (base * bytesPerRow + x >= objectSize)
      break;
  }
}

AOutputBuffer& ADebugDumpable::indent(AOutputBuffer& target, size_t indent)
{
  while (indent > 0)
  {
    target.append(AConstant::ASTRING_TWOSPACES);
    --indent;
  }

  return target;
}

std::ostream& ADebugDumpable::indent(std::ostream& os, size_t indent)
{
  while (indent > 0)
  {
	  os << "  ";
    --indent;
  }

  return os;
}

bool ADebugDumpable::isPointerValid(void *p)
{
#if defined(__WINDOWS__) && defined(_DEBUG)
  switch(sizeof(p))
  {
    case 4 :
    {
      u4 mem = reinterpret_cast<u4>(p);
      if (
        mem == 0xcdcdcdcd ||    // MSVC: Clean land memory. New objects are filled with 0xCD when they are allocated.
        mem == 0xfdfdfdfd ||    // MSVC: No-man's land memory. Extra bytes that belong to the internal block allocated, but not the block you requested. They are placed before and after requested blocks and used for data bound checking.
        mem == 0xdddddddd ||    // MSVC: Dead land memory. The freed blocks kept unused in the debug heap's linked list when the _CRTDBG_DELAY_FREE_MEM_DF flag is set are currently filled with 0xDD. Although in some cases you won't see magic 0xDDDDDDDD value, as it will be overwritten by another debug function (e.g. 0xFEEEFEEE for HeapFree).
        mem == 0xcccccccc ||    // MSVC: The /GX Microsoft Visual C++ compiler option initialises all local variables not explicitly initialised by the program. It fills all memory used by these variables with 0xCC, 0xCCCCCCCC.
        mem == 0xfeeefeee ||    // WINNT: OS fill heap memory, which was marked for usage, but wasn't allocated by HeapAlloc() or LocalAlloc(). Or that memory just has been freed by HeapFree()
        mem == 0xabababab ||    // WINNT: Memory following a block allocated by LocalAlloc()
        mem == 0xbaadf00d       // WINNT: Allocated by LocalAlloc and never initialized
      )
      {
        return false;
      }
    }
    break;

    case 8:
    {
      u8 mem = reinterpret_cast<u8>(p);
      if (
        mem == 0xcdcdcdcdcdcdcdcd ||    // MSVC: Clean land memory. New objects are filled with 0xCD when they are allocated.
        mem == 0xfdfdfdfdfdfdfdfd ||    // MSVC: No-man's land memory. Extra bytes that belong to the internal block allocated, but not the block you requested. They are placed before and after requested blocks and used for data bound checking.
        mem == 0xdddddddddddddddd ||    // MSVC: Dead land memory. The freed blocks kept unused in the debug heap's linked list when the _CRTDBG_DELAY_FREE_MEM_DF flag is set are currently filled with 0xDD. Although in some cases you won't see magic 0xDDDDDDDD value, as it will be overwritten by another debug function (e.g. 0xFEEEFEEE for HeapFree).
        mem == 0xcccccccccccccccc ||    // MSVC: The /GX Microsoft Visual C++ compiler option initialises all local variables not explicitly initialised by the program. It fills all memory used by these variables with 0xCC, 0xCCCCCCCC.
        mem == 0xfeeefeeefeeefeee ||    // WINNT: OS fill heap memory, which was marked for usage, but wasn't allocated by HeapAlloc() or LocalAlloc(). Or that memory just has been freed by HeapFree()
        mem == 0xabababababababab ||    // WINNT: Memory following a block allocated by LocalAlloc()
        mem == 0xbaadf00dbaadf00d       // WINNT: Allocated by LocalAlloc and never initialized
      )
      {
        return false;
      }
    }
    break;

    default:
      ATHROW(NULL, AException::ProgrammingError);  //a_Neither 32 or 64 bit?
  }
#endif

  return true;
}
