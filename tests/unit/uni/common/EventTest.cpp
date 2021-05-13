#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "uni/common/concurrency/ThreadPool.hpp"
#include "uni/common/event/EventListener.hpp"
#include "uni/common/event/Factory.hpp"
#include "uni/common/event/IDispatcher.hpp"
#include "uni/common/event/IEventReceiver.hpp"
#include "uni/common/event/IEventSender.hpp"

namespace integration
{
namespace uni
{
namespace common
{
class EventTest : public ::testing::Test
{
};

}  // namespace common
}  // namespace uni
}  // namespace integration

using integration::uni::common::EventTest;

DECLARE_EVENT_AND_DATA_1( Ping, std::string, m_data );

class MockEventListener : public PingListener
{
    using IDispatcher = ::uni::common::event::IDispatcher;

public:
    explicit MockEventListener( IDispatcher& dispatcher )
        : PingListener{ dispatcher }
    {
    }

public:
    MOCK_METHOD( void, process_event, (const PingEvent&), ( override ) );
};

TEST_F( EventTest, test_send_and_receive_event )
{
    using namespace ::uni::common::event;

    auto receiver_thread = create_event_thread( );
    auto dispatcher = create_event_dispatcher( );

    const auto expected = std::string{ "EventX" };

    const auto event_receiver = creare_event_receiver( *dispatcher, receiver_thread );

    MockEventListener event_listener{ *dispatcher };
    ON_CALL( event_listener, process_event( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke( [ expected ]( const PingEvent& event ) {
            ASSERT_EQ( expected, event.get_data( ).m_data );
        } ) );
    EXPECT_CALL( event_listener, process_event( ::testing::_ ) ).Times( 1 );

    receiver_thread.start( );

    auto event = std::make_shared< PingEvent >( Ping{ expected } );
    auto event_sender = create_event_sender( event_receiver );
    event_sender->send( std::move( event ) );

    receiver_thread.stop( );
}

// struct D
//{
//    std::string data;
//};
// TEST_F( EventTest, test_event_data_move_copy )
//{
//    using namespace ::uni::common::event;

//    D d{ "str1" };
//    TEvent< D > e1{ d };
//    std::cerr << "d1=" << e1.get_data( ).data << '\n';

//    TEvent< D > e2 = std::move( e1 );
//    std::cerr << "d2=" << e2.get_data( ).data << '\n';
//    std::cerr << "d1=" << e1.get_data( ).data << '\n';

//    {
//        auto ptr = std::make_unique< TEvent< D > >( d );
//        auto ptr2 = std::move( ptr );
//    }
//}
