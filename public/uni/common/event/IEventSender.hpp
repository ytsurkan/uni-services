#pragma once

#include <memory>

// forward declaration
namespace uni
{
namespace common
{
namespace event
{
class IEvent;
}  // namespace event
}  // namespace common
}  // namespace uni

namespace uni
{
namespace common
{
namespace event
{
class IEventSender
{
public:
    IEventSender( ) = default;
    IEventSender( const IEventSender& ) = default;
    IEventSender& operator=( const IEventSender& ) = default;
    IEventSender( IEventSender&& ) = default;
    IEventSender& operator=( IEventSender&& ) = default;
    virtual ~IEventSender( ) = default;

public:
    virtual void send( std::shared_ptr< const IEvent > event ) = 0;
};
}  // namespace event
}  // namespace common
}  // namespace uni
