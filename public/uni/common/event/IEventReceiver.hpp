#pragma once

#include <memory>

#include "uni/common/event/FwdDeclarations.hpp"

namespace uni
{
namespace common
{
namespace event
{
class IEventReceiver
{
public:
    IEventReceiver( ) = default;
    IEventReceiver( const IEventReceiver& ) = default;
    IEventReceiver& operator=( const IEventReceiver& ) = default;
    IEventReceiver( IEventReceiver&& ) = default;
    IEventReceiver& operator=( IEventReceiver&& ) = default;
    virtual ~IEventReceiver( ) = default;

public:
    virtual void receive( std::shared_ptr< const IEvent > ) = 0;
};

}  // namespace event
}  // namespace common
}  // namespace uni
