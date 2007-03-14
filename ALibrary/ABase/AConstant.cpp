#include "pchABase.hpp"
#include "AConstant.hpp"
#include <limits.h>

const size_t AConstant::npos(-1);

#if defined(_WIN64)
const size_t AConstant::MAX_SIZE_T(_UI64_MAX);  // 0xffffffffffffffffui64
#elif defined(_WIN32)
const size_t AConstant::MAX_SIZE_T(_UI32_MAX);  // 0xffffffffui32
#else
#pragma error("Unknown size_t max size")
#endif