#include "uni/common/event/Factory.hpp"

#include "uni/common/event/Dispatcher.hpp"
#include "uni/common/event/EventReceiver.hpp"
#include "uni/common/event/EventSender.hpp"

namespace uni
{
namespace common
{
namespace event
{
EventThread
create_event_thread( )
{
    return ::uni::common::event::EventThread{ 1u };
}

std::shared_ptr< IEventReceiver >
creare_event_receiver( IDispatcher& dispatcher, EventThread& thread )
{
    return std::make_shared< EventReceiver >( dispatcher, thread );
}

std::unique_ptr< IEventSender >
create_event_sender( std::weak_ptr< IEventReceiver > event_receiver )
{
    return std::make_unique< EventSender >( event_receiver );
}

std::unique_ptr< IDispatcher >
create_event_dispatcher( )
{
    return std::make_unique< Dispatcher >( );
}

}  // namespace event
}  // namespace common
}  // namespace uni
