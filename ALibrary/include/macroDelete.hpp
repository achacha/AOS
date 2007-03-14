#ifndef INCLUDED__macroDelete_HPP__
#define INCLUDED__macroDelete_HPP__

#define pDelete(ptr) \
  do { delete ptr; ptr = NULL; } while(false)
#define pDeleteArray(ptr) \
  do { delete[] ptr; ptr = NULL; } while(false)

#endif

