#ifndef TYPEDEFS_H_INCLUDED
#define TYPEDEFS_H_INCLUDED

#include <cstddef>
#include <vector>
#include <ChangeInterface.h>
#include <google/protobuf/stubs/common.h>

using google::protobuf::uint32;
using google::protobuf::int32;

typedef uint32 TileId;
typedef int32 UnitId;
typedef std::vector<ChangeInterface*> ChangeList;

#endif // TYPEDEFS_H_INCLUDED
