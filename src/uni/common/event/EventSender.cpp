
#include "uni/common/event/EventSender.hpp"

#include "uni/common/event/IEvent.hpp"
#include "uni/common/event/IEventReceiver.hpp"

namespace uni
{
namespace common
{
namespace event
{
void
EventSender::send( std::shared_ptr< const IEvent > event )
{
    if ( const auto& shared_event_receiver = m_event_receiver.lock( ) )
    {
        shared_event_receiver->receive( std::move( event ) );
    }
}

}  // namespace event
}  // namespace common
}  // namespace uni
