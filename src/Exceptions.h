#ifndef EXCEPTIONS_H_INCLUDED
#define EXCEPTIONS_H_INCLUDED
#include <boost/exception/all.hpp>

struct ClientBehind: virtual std::exception, virtual boost::exception { };

#endif // EXCEPTIONS_H_INCLUDED
