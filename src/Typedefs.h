#ifndef TYPEDEFS_H_INCLUDED
#define TYPEDEFS_H_INCLUDED

#include <google/protobuf/stubs/common.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <libintl.h>
#define _(String) gettext (String)

using google::protobuf::uint32;
using google::protobuf::uint64;
using google::protobuf::int32;
using google::protobuf::int64;

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLStream;
typedef boost::shared_ptr< SSLStream > SSLStreamPtr;

typedef uint32 TileId;
typedef int32 UnitId;
typedef uint64 GameTime;
typedef int64 Miliseconds;

#endif // TYPEDEFS_H_INCLUDED
