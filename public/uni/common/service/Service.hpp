#pragma once

#include "uni/common/Logger.hpp"
#include "uni/common/event/Event.hpp"
#include "uni/common/event/EventListener.hpp"
#include "uni/common/event/IDispatcher.hpp"
#include "uni/common/event/IEventSender.hpp"

namespace uni
{
namespace common
{
namespace service
{
template < typename TRequest, typename TResponse >
class Service
{
public:
    using SenderType = ::uni::common::event::IEventSender;
    using DispatcherType = ::uni::common::event::IDispatcher;

    class Client : public ::uni::common::event::EventListener< TResponse >
    {
    public:
        Client( std::unique_ptr< SenderType > sender, DispatcherType& dispatcher )
            : ::uni::common::event::EventListener< TResponse >{ dispatcher }
            , m_sender{ std::move( sender ) }
        {
        }

        virtual void process_response( const TResponse& response ) = 0;

    protected:
        void
        send_request( const TRequest& request )
        {
            if ( !m_sender )
            {
                LOG_ERROR( "Unexpected nullptr sender" );
                return;
            }
            ::uni::common::event::Event< TRequest >::broadcast_event( request, *m_sender );
        }

    private:
        void
        process_event( const ::uni::common::event::Event< TResponse >& event ) override
        {
            process_response( event.get_data( ) );
        }

    private:
        std::unique_ptr< SenderType > m_sender;
    };

    class Server : public ::uni::common::event::EventListener< TRequest >
    {
    public:
        Server( std::unique_ptr< SenderType > sender, DispatcherType& dispatcher )
            : ::uni::common::event::EventListener< TRequest >{ dispatcher }
            , m_sender{ std::move( sender ) }
        {
        }

        virtual void process_request( const TRequest& request ) = 0;

    protected:
        void
        send_response( const TResponse& response )
        {
            if ( !m_sender )
            {
                LOG_ERROR( "Unexpected nullptr sender" );
                return;
            }
            ::uni::common::event::Event< TResponse >::broadcast_event( response, *m_sender );
        }

    private:
        void
        process_event( const ::uni::common::event::Event< TRequest >& event ) override
        {
            process_request( event.get_data( ) );
        }

    private:
        std::unique_ptr< SenderType > m_sender;
    };
};

}  // namespace service
}  // namespace common
}  // namespace uni
