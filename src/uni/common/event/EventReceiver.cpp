#include "uni/common/event/EventReceiver.hpp"

#include "uni/common/event/IDispatcher.hpp"
#include "uni/common/event/IEvent.hpp"

namespace uni
{
namespace common
{
namespace event
{
EventReceiver::EventReceiver( IDispatcher& dispatcher, EventThread& thread )
    : m_dispatcher{ dispatcher }
    , m_thread{ thread }
{
}

void
EventReceiver::receive( std::shared_ptr< const IEvent > event )
{
    m_thread.add_task( [ this, event = std::move( event ) ]( ) {
        // TODO and log: assert( nullptr != event.get( ) );
        m_dispatcher.dispatch( *event );
    } );
}

}  // namespace event
}  // namespace common
}  // namespace uni
