#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "uni/common/concurrency/ThreadPool.hpp"
#include "uni/common/event/EventListener.hpp"
#include "uni/common/event/Factory.hpp"
#include "uni/common/event/IDispatcher.hpp"
#include "uni/common/event/IEventReceiver.hpp"
#include "uni/common/event/IEventSender.hpp"
#include "uni/common/service/Service.hpp"

namespace integration
{
namespace uni
{
namespace common
{
class ServiceTest : public ::testing::Test
{
};

}  // namespace common
}  // namespace uni
}  // namespace integration

using integration::uni::common::ServiceTest;

using IEventSender = ::uni::common::event::IEventSender;
using IDispatcher = ::uni::common::event::IDispatcher;
using IEventReceiver = ::uni::common::event::IEventReceiver;

struct Request_1
{
    std::string m_data;
};

struct Response_1
{
    std::string m_data;
};

using EchoService = ::uni::common::service::Service< Request_1, Response_1 >;

class MockClient : public EchoService::Client
{
public:
    MockClient( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoService::Client{ std::move( event_sender ), dispatcher }
    {
    }

    void
    send_request_1( const Request_1& request )
    {
        EchoService::Client::send_request( request );
    }

    MOCK_METHOD( void, process_response, (const Response_1&), ( override ) );
};

class MockService : public EchoService::Server
{
public:
    MockService( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoService::Server{ std::move( event_sender ), dispatcher }
    {
    }

    void
    send_response_1( const Response_1& response )
    {
        EchoService::Server::send_response( response );
    }

    MOCK_METHOD( void, process_request, (const Request_1&), ( override ) );
};

TEST_F( ServiceTest, test_service_send_request )
{
    using namespace ::uni::common::event;

    int* p = new int;
    (void)p;

    auto receiver_thread = create_event_thread( );
    auto dispatcher = create_event_dispatcher( );
    auto event_receiver = creare_event_receiver( *dispatcher, receiver_thread );

    const auto expected = std::string{ "request_1" };

    MockClient client{ create_event_sender( event_receiver ), *dispatcher };
    MockService service{ create_event_sender( event_receiver ), *dispatcher };
    ON_CALL( service, process_request( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke(
            [ expected ]( const Request_1& event ) { ASSERT_EQ( expected, event.m_data ); } ) );
    EXPECT_CALL( service, process_request( ::testing::_ ) ).Times( 1 );

    receiver_thread.start( );

    client.send_request_1( { expected } );

    receiver_thread.stop( );
}

TEST_F( ServiceTest, test_service_send_response )
{
    using namespace ::uni::common::event;

    auto receiver_thread = create_event_thread( );
    auto dispatcher = create_event_dispatcher( );
    auto event_receiver = creare_event_receiver( *dispatcher, receiver_thread );

    const auto expected = std::string{ "response_1" };

    MockClient client{ create_event_sender( event_receiver ), *dispatcher };
    ON_CALL( client, process_response( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke(
            [ expected ]( const Response_1& event ) { ASSERT_EQ( expected, event.m_data ); } ) );
    EXPECT_CALL( client, process_response( ::testing::_ ) ).Times( 1 );
    MockService service{ create_event_sender( event_receiver ), *dispatcher };

    receiver_thread.start( );

    service.send_response_1( { expected } );

    receiver_thread.stop( );
}

