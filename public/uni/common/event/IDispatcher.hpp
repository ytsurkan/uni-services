#pragma once

namespace uni
{
namespace common
{
namespace event
{
class IEventListener;

class IEvent;

class IDispatcher
{
public:
    IDispatcher( ) = default;
    IDispatcher( const IDispatcher& ) = delete;
    IDispatcher& operator=( const IDispatcher& ) = delete;
    IDispatcher( IDispatcher&& ) = default;
    IDispatcher& operator=( IDispatcher&& ) = default;
    virtual ~IDispatcher( ) = default;

public:
    virtual void dispatch( const IEvent& event ) = 0;
    virtual void register_listener( IEventListener* listener ) = 0;
    virtual void unregister_listener( IEventListener* listener ) = 0;
};

}  // namespace event
}  // namespace common
}  // namespace uni
