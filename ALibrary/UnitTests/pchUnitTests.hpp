/*
Written by Alex Chachanashvili

$Id$
*/
//#define WINDOWS_CRTDBG_ENABLED 1
#include "apiABase.hpp"
#include <AException.hpp>
#include <AString.hpp>

#define ASSERT_UNIT_TEST(ut, error_message, error_data, error_count) { \
  if (!(ut)) { \
    ++error_count;\
    std::cerr << error_message;\
    std::cerr << "[" << error_data << "]";\
    std::cerr << std::endl;\
  } else { std::cerr << "." << std::flush; } \
}

#define NEWLINE_UNIT_TEST() { std::cerr << std::endl; }
