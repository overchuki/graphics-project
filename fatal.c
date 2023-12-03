//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx229.h"

//
//  Print message to stderr and exit
//
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}
