#ifndef WARNINGS_H
#define WARNINGS_H

// Pragma tricks are used here *only* to bring vc++ and gcc
// closer to each other in terms of produced warinigs
#ifdef _MSC_VER
#pragma warning(disable: 4800) // conversion from 'float' to 'int', possible loss of data
#pragma warning(disable: 4244) // forcing value to bool 'true' or 'false'
#pragma warning(disable: 4996) // The POSIX name for this item is deprecated
#pragma warning(disable: 4068) // Unknown pragma
#pragma warning(default: 4239) // nonstandard extension used ... A non-const reference may only be bound to an lvalue
#endif

#ifdef __GNUC__
#pragma GCC diagnostic warning "-Wunused-variable"
#pragma GCC diagnostic warning "-Wreturn-type"
#pragma GCC diagnostic warning "-Wsign-compare"
#endif

#endif
