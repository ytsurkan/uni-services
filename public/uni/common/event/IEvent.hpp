#pragma once

#include <string>

namespace uni
{
namespace common
{
namespace event
{
class IEvent
{
public:
    IEvent( ) = default;
    IEvent( const IEvent& ) = default;
    IEvent& operator=( const IEvent& ) = default;
    IEvent( IEvent&& ) = default;
    IEvent& operator=( IEvent&& ) = default;
    virtual ~IEvent( ) = default;

public:
    virtual size_t get_type_id( ) const = 0;
    virtual const std::string& get_type_name( ) const = 0;
};

}  // namespace event
}  // namespace common
}  // namespace uni
