#pragma once

#include "uni/common/event/EventThread.hpp"
#include "uni/common/event/FwdDeclarations.hpp"

namespace uni
{
namespace common
{
namespace event
{
EventThread create_event_thread( );
std::unique_ptr< IDispatcher > create_event_dispatcher( );
std::shared_ptr< IEventReceiver > creare_event_receiver( IDispatcher& dispatcher,
                                                         EventThread& thread );
std::unique_ptr< IEventSender > create_event_sender(
    std::weak_ptr< IEventReceiver > event_receiver );

}  // namespace event
}  // namespace common
}  // namespace uni
