#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "uni/common/concurrency/ThreadPool.hpp"
#include "uni/common/event/Factory.hpp"
#include "uni/common/event/IDispatcher.hpp"
#include "uni/common/event/IEventReceiver.hpp"
#include "uni/common/event/IEventSender.hpp"
#include "uni/common/service/Broadcast.hpp"

namespace integration
{
namespace uni
{
namespace common
{
class BroadcastTest : public ::testing::Test
{
};

}  // namespace common
}  // namespace uni
}  // namespace integration

using integration::uni::common::BroadcastTest;

struct BroadcastData_1
{
    std::string m_data;
};
using Broadcast_1 = ::uni::common::service::Broadcast< BroadcastData_1 >;

using IEventSender = ::uni::common::event::IEventSender;
using IDispatcher = ::uni::common::event::IDispatcher;

class BroadcastSender : public Broadcast_1::Sender
{
public:
    explicit BroadcastSender( std::unique_ptr< IEventSender > event_sender )
        : Broadcast_1::Sender{ std::move( event_sender ) }
    {
    }

    void
    send_broadcast_1( const BroadcastData_1& data )
    {
        send_broadcast< Broadcast_1 >( data );
    }

private:
    template < typename TBroadcast >
    void
    send_broadcast( const typename TBroadcast::BroadcastDataType& data )
    {
        TBroadcast::Sender::notify_all( data );
    }
};

class MockBroadcastReceiver : public Broadcast_1::Receiver
{
public:
    explicit MockBroadcastReceiver( IDispatcher& dispatcher )
        : Broadcast_1::Receiver{ dispatcher }
    {
    }

    MOCK_METHOD( void, process_notification, (const BroadcastData_1&), ( override ) );
};

TEST_F( BroadcastTest, test_broadcast_sender_and_receiver )
{
    using namespace ::uni::common::event;
    using namespace ::uni::common::service;

    auto receiver_thread = create_event_thread( );
    auto dispatcher = create_event_dispatcher( );

    auto event_receiver = creare_event_receiver( *dispatcher, receiver_thread );

    const auto expected = std::string{ "broadcast_1" };

    MockBroadcastReceiver broadcast_receiver{ *dispatcher };
    ON_CALL( broadcast_receiver, process_notification( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke( [ expected ]( const BroadcastData_1& event ) {
            ASSERT_EQ( expected, event.m_data );
        } ) );
    EXPECT_CALL( broadcast_receiver, process_notification( ::testing::_ ) ).Times( 1 );

    BroadcastSender broadcast_sender{ create_event_sender( event_receiver ) };

    receiver_thread.start( );

    broadcast_sender.send_broadcast_1( { expected } );

    receiver_thread.stop( );
}

