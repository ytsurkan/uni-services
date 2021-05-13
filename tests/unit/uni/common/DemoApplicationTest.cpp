#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <thread>

#include "uni/common/Logger.hpp"
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
class DemoApplicationTest : public ::testing::Test
{
};

}  // namespace common
}  // namespace uni
}  // namespace integration

using integration::uni::common::DemoApplicationTest;

DECLARE_EVENT_AND_DATA_1( Echo, std::string, m_data );

using IEventSender = ::uni::common::event::IEventSender;
using IDispatcher = ::uni::common::event::IDispatcher;
using IEventReceiver = ::uni::common::event::IEventReceiver;

class SDKListener : public EchoListener
{
public:
    explicit SDKListener( IDispatcher& dispatcher )
        : EchoListener{ dispatcher }
    {
    }

private:
    void
    process_event( const EchoEvent& ) override
    {
        // TODO: add some logic
    }
};

class MockSDKListener : public SDKListener
{
public:
    explicit MockSDKListener( IDispatcher& dispatcher )
        : SDKListener{ dispatcher }
    {
    }

public:
    MOCK_METHOD( void, process_event, (const EchoEvent&), ( override ) );
};

class SDKComponent
{
public:
    SDKComponent( std::unique_ptr< IEventSender > event_sender )
        : m_event_sender{ std::move( event_sender ) }
    {
    }

    void
    echo_event_1( const std::string& data )
    {
        auto event = std::make_shared< EchoEvent >( Echo{ data } );
        // TODO: add assert assert(m_event_sender != nullptr)
        m_event_sender->send( std::move( event ) );
    }

private:
    std::unique_ptr< IEventSender > m_event_sender;
};

using EventThread = ::uni::common::event::EventThread;

struct Request_1
{
    std::string m_data;
    LOG_STRUCT( Request_1, LOG_ITEM( m_data ) );
};

struct Response_1
{
    std::string m_data;
    LOG_STRUCT( Response_1, LOG_ITEM( m_data ) );
};

using EchoService = ::uni::common::service::Service< Request_1, Response_1 >;

class EchoServiceServer : public EchoService::Server
{
public:
    EchoServiceServer( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoService::Server{ std::move( event_sender ), dispatcher }
    {
    }

    void
    send_response_1( const Response_1& response )
    {
        LOG_DEBUG( LOG_ITEM( "response", response ) );
        EchoService::Server::send_response( response );
    }

    void
    process_request( const Request_1& request ) override
    {
        send_response_1( { request.m_data } );
    }
};

class MockEchoServiceServer : public EchoServiceServer
{
public:
    MockEchoServiceServer( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoServiceServer{ std::move( event_sender ), dispatcher }
    {
    }

    MOCK_METHOD( void, process_request, (const Request_1&), ( override ) );
};

class EchoServiceClient : public EchoService::Client
{
public:
    EchoServiceClient( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoService::Client{ std::move( event_sender ), dispatcher }
    {
    }

    void
    send_request_1( const Request_1& request )
    {
        LOG_DEBUG( LOG_ITEM( "request", request ) );
        EchoService::Client::send_request( request );
    }

    void
    process_response( const Response_1& response ) override
    {
        send_request_1( { response.m_data } );
    }
};

class MockEchoServiceClient : public EchoServiceClient
{
public:
    MockEchoServiceClient( std::unique_ptr< IEventSender > event_sender, IDispatcher& dispatcher )
        : EchoServiceClient{ std::move( event_sender ), dispatcher }
    {
    }

    MOCK_METHOD( void, process_response, (const Response_1&), ( override ) );
};

class CtrlComponent
{
public:
    explicit CtrlComponent( EventThread& thread )
        : m_dispatcher{ ::uni::common::event::create_event_dispatcher( ) }
        , m_event_receiver{ creare_event_receiver( *m_dispatcher, thread ) }
    {
    }

    IDispatcher&
    get_dispatcher( )
    {
        return *m_dispatcher;
    }

    std::weak_ptr< IEventReceiver >
    get_event_receiver( ) const
    {
        return m_event_receiver;
    }

    void
    set_sdk_listener( std::unique_ptr< SDKListener > sdk_listener )
    {
        m_sdk_listener = std::move( sdk_listener );
    }

    void
    set_echoservice_server( std::unique_ptr< EchoServiceServer > service_1 )
    {
        m_echoservice_server = std::move( service_1 );
    }

    void
    send_response_1( const Response_1& response )
    {
        if ( !m_echoservice_server )
        {
            return;
        }
        m_echoservice_server->send_response_1( response );
    }

private:
    std::unique_ptr< IDispatcher > m_dispatcher;
    std::shared_ptr< IEventReceiver > m_event_receiver;
    std::unique_ptr< SDKListener > m_sdk_listener{ };
    std::unique_ptr< EchoServiceServer > m_echoservice_server{ };
};

class UIComponent
{
public:
    explicit UIComponent( EventThread& thread )
        : m_dispatcher{ ::uni::common::event::create_event_dispatcher( ) }
        , m_event_receiver{ creare_event_receiver( *m_dispatcher, thread ) }
    {
    }

    IDispatcher&
    get_dispatcher( )
    {
        return *m_dispatcher;
    }

    std::weak_ptr< IEventReceiver >
    get_event_receiver( ) const
    {
        return m_event_receiver;
    }

    void
    set_echoservice_client( std::unique_ptr< EchoServiceClient > service_1_client )
    {
        m_echoservice_client = std::move( service_1_client );
    }

public:
    void
    send_request_1( const Request_1& request )
    {
        if ( !m_echoservice_client )
        {
            return;
        }
        m_echoservice_client->send_request_1( request );
    }

private:
    std::unique_ptr< IDispatcher > m_dispatcher;
    std::shared_ptr< IEventReceiver > m_event_receiver;
    std::unique_ptr< EchoServiceClient > m_echoservice_client{ };
};

class DemoApplication
{
public:
    DemoApplication( )
        : m_ctrl_thread{ ::uni::common::event::create_event_thread( ) }
        , m_ui_thread{ ::uni::common::event::create_event_thread( ) }
    {
    }

    void
    start( )
    {
        if ( m_started )
        {
            throw std::logic_error( "DemoApplication already started!" );
        }
        m_started = true;

        create_components( );
        m_ctrl_thread.start( );
        m_ui_thread.start( );
    }

    void
    stop( )
    {
        if ( !m_started )
        {
            throw std::logic_error( "DemoApplication already stopped!" );
        }
        m_started = false;

        m_ctrl_thread.stop( );
        m_ui_thread.stop( );

        reset_components( );
    }

    void
    send_event_from_sdk_to_ctrl( )
    {
        const auto expected = std::string{ "SDKEvent_1" };
        m_ctrl->set_sdk_listener( create_mock_sdk_listener( expected ) );
        m_sdk->echo_event_1( expected );
    }

    void
    ctrl_send_response_to_ui( )
    {
        const auto expected = std::string{ "response_1" };
        m_ui->set_echoservice_client( create_mock_client_of_echoservice( expected ) );
        m_ctrl->set_echoservice_server( create_server_of_echoservice( ) );

        m_ctrl->send_response_1( { expected } );
    }

    void
    ui_send_request_to_ctrl( )
    {
        const auto expected = std::string{ "request_1" };
        m_ui->set_echoservice_client( create_client_of_echoservice( ) );
        m_ctrl->set_echoservice_server( create_mock_server_of_echoservice( expected ) );

        m_ui->send_request_1( { expected } );
    }

private:
    void
    create_components( )
    {
        using ::uni::common::event::create_event_sender;
        m_ctrl = std::make_unique< CtrlComponent >( m_ctrl_thread );
        m_ui = std::make_unique< UIComponent >( m_ui_thread );
        m_sdk = std::make_unique< SDKComponent >(
            create_event_sender( m_ctrl->get_event_receiver( ) ) );
    }

    void
    reset_components( )
    {
        m_ctrl.reset( );
        m_ui.reset( );
        m_sdk.reset( );
    }

private:
    std::unique_ptr< MockSDKListener >
    create_mock_sdk_listener( const std::string& expected )
    {
        auto event_listener = std::make_unique< MockSDKListener >( m_ctrl->get_dispatcher( ) );
        ON_CALL( *event_listener, process_event( ::testing::_ ) )
            .WillByDefault( ::testing::Invoke( [ expected ]( const EchoEvent& event ) {
                ASSERT_EQ( expected, event.get_data( ).m_data );
            } ) );
        EXPECT_CALL( *event_listener, process_event( ::testing::_ ) ).Times( 1 );
        return event_listener;
    }

    std::unique_ptr< EchoServiceClient >
    create_client_of_echoservice( )
    {
        auto client = std::make_unique< EchoServiceClient >(
            create_event_sender( m_ctrl->get_event_receiver( ) ), m_ui->get_dispatcher( ) );
        return client;
    }

    std::unique_ptr< EchoServiceServer >
    create_server_of_echoservice( )
    {
        auto service = std::make_unique< EchoServiceServer >(
            create_event_sender( m_ui->get_event_receiver( ) ), m_ctrl->get_dispatcher( ) );
        return service;
    }

    std::unique_ptr< EchoServiceClient >
    create_mock_client_of_echoservice( const std::string& expected )
    {
        auto mock_client = std::make_unique< MockEchoServiceClient >(
            create_event_sender( m_ctrl->get_event_receiver( ) ), m_ui->get_dispatcher( ) );

        ON_CALL( *mock_client, process_response( ::testing::_ ) )
            .WillByDefault( ::testing::Invoke( [ expected ]( const Response_1& event ) {
                ASSERT_EQ( expected, event.m_data );
            } ) );
        EXPECT_CALL( *mock_client, process_response( ::testing::_ ) ).Times( 1 );
        return mock_client;
    }

    std::unique_ptr< MockEchoServiceServer >
    create_mock_server_of_echoservice( const std::string& expected )
    {
        auto mock_server = std::make_unique< MockEchoServiceServer >(
            create_event_sender( m_ui->get_event_receiver( ) ), m_ctrl->get_dispatcher( ) );
        ON_CALL( *mock_server, process_request( ::testing::_ ) )
            .WillByDefault( ::testing::Invoke(
                [ expected ]( const Request_1& event ) { ASSERT_EQ( expected, event.m_data ); } ) );
        EXPECT_CALL( *mock_server, process_request( ::testing::_ ) ).Times( 1 );
        return mock_server;
    }

private:
    EventThread m_ctrl_thread;
    EventThread m_ui_thread;
    std::atomic< bool > m_started{ false };
    std::unique_ptr< CtrlComponent > m_ctrl{ };
    std::unique_ptr< UIComponent > m_ui{ };
    std::unique_ptr< SDKComponent > m_sdk{ };
};

TEST_F( DemoApplicationTest, test_ctrl_receive_event )
{
    using namespace ::uni::common::event;

    auto ctrl_thread = create_event_thread( );

    CtrlComponent ctrl{ ctrl_thread };

    const auto expected = std::string{ "SDKEvent_1" };

    auto sdk_event_listener = std::make_unique< MockSDKListener >( ctrl.get_dispatcher( ) );
    ON_CALL( *sdk_event_listener, process_event( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke( [ expected ]( const EchoEvent& event ) {
            ASSERT_EQ( expected, event.get_data( ).m_data );
        } ) );
    EXPECT_CALL( *sdk_event_listener, process_event( ::testing::_ ) ).Times( 1 );

    ctrl.set_sdk_listener( std::move( sdk_event_listener ) );

    SDKComponent sdk_component{ create_event_sender( ctrl.get_event_receiver( ) ) };

    ctrl_thread.start( );

    sdk_component.echo_event_1( expected );

    ctrl_thread.stop( );
}

TEST_F( DemoApplicationTest, test_echoservice_send_response )
{
    using namespace ::uni::common::event;

    auto ctrl_thread = create_event_thread( );
    CtrlComponent ctrl{ ctrl_thread };

    auto ui_thread = create_event_thread( );
    UIComponent ui{ ui_thread };

    const auto expected = std::string{ "response_1" };

    auto client = std::make_unique< MockEchoServiceClient >(
        create_event_sender( ctrl.get_event_receiver( ) ), ui.get_dispatcher( ) );
    ON_CALL( *client, process_response( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke(
            [ expected ]( const Response_1& event ) { ASSERT_EQ( expected, event.m_data ); } ) );
    EXPECT_CALL( *client, process_response( ::testing::_ ) ).Times( 1 );
    ui.set_echoservice_client( std::move( client ) );
    MockEchoServiceServer service{ create_event_sender( ui.get_event_receiver( ) ),
                                   ctrl.get_dispatcher( ) };

    ctrl_thread.start( );
    ui_thread.start( );

    service.send_response_1( { expected } );

    ctrl_thread.stop( );
    ui_thread.stop( );
}

TEST_F( DemoApplicationTest, test_echoservice_send_request )
{
    using namespace ::uni::common::event;

    auto ui_thread = create_event_thread( );
    UIComponent ui{ ui_thread };

    auto ctrl_thread = create_event_thread( );
    CtrlComponent ctrl{ ctrl_thread };

    const auto expected = std::string{ "request_1" };

    MockEchoServiceClient client{ create_event_sender( ctrl.get_event_receiver( ) ),
                                  ui.get_dispatcher( ) };
    auto service = std::make_unique< MockEchoServiceServer >(
        create_event_sender( ui.get_event_receiver( ) ), ctrl.get_dispatcher( ) );
    ON_CALL( *service, process_request( ::testing::_ ) )
        .WillByDefault( ::testing::Invoke(
            [ expected ]( const Request_1& event ) { ASSERT_EQ( expected, event.m_data ); } ) );
    EXPECT_CALL( *service, process_request( ::testing::_ ) ).Times( 1 );
    ctrl.set_echoservice_server( std::move( service ) );

    ui_thread.start( );
    ctrl_thread.start( );

    client.send_request_1( { expected } );

    ui_thread.stop( );
    ctrl_thread.stop( );
}

TEST_F( DemoApplicationTest, send_event_from_sdk_to_ctrl )
{
    DemoApplication app;
    app.start( );

    app.send_event_from_sdk_to_ctrl( );

    app.stop( );
}

TEST_F( DemoApplicationTest, ctrl_send_response_to_ui )
{
    DemoApplication app;
    app.start( );

    app.ctrl_send_response_to_ui( );

    app.stop( );
}

TEST_F( DemoApplicationTest, ui_send_request_to_ctrl )
{
    DemoApplication app;
    app.start( );

    app.ui_send_request_to_ctrl( );

    app.stop( );
}
