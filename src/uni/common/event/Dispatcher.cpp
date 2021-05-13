#include "uni/common/event/Dispatcher.hpp"

#include <algorithm>
#include <iostream>

#include "uni/common/Logger.hpp"
#include "uni/common/event/EventListener.hpp"
#include "uni/common/event/IEvent.hpp"

namespace uni
{
namespace common
{
namespace event
{
void
Dispatcher::dispatch( const IEvent& event )
{
    Listeners listeners;
    if ( get_listeners( event.get_type_id( ), listeners ) )
    {
        for ( IEventListener* listener : listeners )
        {
            listener->process_event( event );
        }
    }
    else
    {
        LOG_ERROR( "Listener not registered for event id=",
                   event.get_type_id( ),
                   ", type=",
                   event.get_type_name( ) );
    }
}

void
Dispatcher::register_listener( IEventListener* listener )
{
    if ( nullptr == listener )
    {
        LOG_ERROR( "Unexpected nullptr listener" );
        return;
    }
    std::lock_guard< std::mutex > _( m_mutex );

    const auto id = listener->get_type_id( );
    auto& listeners = m_listeners[ id ];
    auto found = std::find( std::begin( listeners ), std::end( listeners ), listener );
    if ( std::end( listeners ) != found )
    {
        LOG_ERROR( "Listener already registered, id=",
                   listener->get_type_id( ),
                   ", type=",
                   listener->get_type_name( ) );
        return;
    }
    listeners.push_back( listener );
}

void
Dispatcher::unregister_listener( IEventListener* listener )
{
    if ( nullptr == listener )
    {
        LOG_ERROR( "Unexpected nullptr listener" );
        return;
    }
    std::lock_guard< std::mutex > _( m_mutex );

    const auto id = listener->get_type_id( );
    auto found = m_listeners.find( id );
    if ( std::end( m_listeners ) == found )
    {
        LOG_ERROR( "Listener with id=", id, ", type=", listener->get_type_name( ), " not found" );
        return;
    }
    auto& listeners = found->second;
    listeners.erase( std::remove( listeners.begin( ), listeners.end( ), listener ),
                     listeners.end( ) );
}

bool
Dispatcher::get_listeners( size_t id, Listeners& listeners )
{
    std::lock_guard< std::mutex > _( m_mutex );
    const auto it = m_listeners.find( id );
    if ( it != m_listeners.end( ) )
    {
        listeners = it->second;
        return true;
    }
    return false;
}

}  // namespace event
}  // namespace common
}  // namespace uni
