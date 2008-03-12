#ifndef INCLUDED__macroDelete_HPP__
#define INCLUDED__macroDelete_HPP__

#define pDelete(ptr) { delete ptr; ptr = NULL; }
#define pDeleteArray(ptr) { delete[] ptr; ptr = NULL; }

#endif

