#pragma once

#include <string>
#include <typeinfo>

namespace uni
{
namespace common
{
namespace event
{
namespace helpers
{
template < typename D >
size_t
type_id( )
{
    return typeid( D ).hash_code( );
}

template < typename D >
const std::string&
type_name( )
{
    static const std::string name = typeid( D ).name( );
    return name;
}

}  // namespace helpers
}  // namespace event
}  // namespace common
}  // namespace uni
