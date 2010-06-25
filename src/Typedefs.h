#ifndef TYPEDEFS_H_INCLUDED
#define TYPEDEFS_H_INCLUDED

#include <google/protobuf/stubs/common.h>
#include <vector>
#include <boost/asio.hpp>
#include <libintl.h>
#define _(String) gettext (String)

using google::protobuf::uint32;
using google::protobuf::uint64;
using google::protobuf::int32;
using google::protobuf::int64;
using boost::asio::ip::tcp;

typedef boost::shared_ptr< tcp::socket > SocketSharedPtr;
typedef uint32 TileId;
typedef int32 UnitId;
typedef uint64 GameTime;

#endif // TYPEDEFS_H_INCLUDED
