#pragma once

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
template < typename TBroadcastDataType >
class Broadcast
{
public:
    using BroadcastDataType = TBroadcastDataType;
    using SenderType = ::uni::common::event::IEventSender;
    using DispatcherType = ::uni::common::event::IDispatcher;

    class Receiver : public ::uni::common::event::EventListener< TBroadcastDataType >
    {
    public:
        explicit Receiver( DispatcherType& dispatcher )
            : ::uni::common::event::EventListener< TBroadcastDataType >{ dispatcher }
        {
        }

        virtual void process_notification( const TBroadcastDataType& broadcast_data ) = 0;

    protected:
        void
        process_event( const ::uni::common::event::Event< TBroadcastDataType >& event ) override
        {
            process_notification( event.get_data( ) );
        }
    };

    class Sender
    {
    public:
        explicit Sender( std::unique_ptr< SenderType > sender )
            : m_sender{ std::move( sender ) }
        {
        }

        Sender( const Sender& ) = delete;
        Sender& operator=( const Sender& ) = delete;

        Sender( Sender&& ) = default;
        Sender& operator=( Sender&& ) = default;
        virtual ~Sender( ) = default;

        void
        notify_all( const TBroadcastDataType& broadcast_data )
        {
            if ( nullptr != m_sender )
            {
                ::uni::common::event::Event< TBroadcastDataType >::broadcast_event( broadcast_data,
                                                                                    *m_sender );
            }
        }

    private:
        std::unique_ptr< SenderType > m_sender;
    };
};

}  // namespace service
}  // namespace common
}  // namespace uni
