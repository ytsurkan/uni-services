#pragma once

#include "uni/common/event/Helpers.hpp"
#include "uni/common/event/IEvent.hpp"
#include "uni/common/event/IEventSender.hpp"

namespace uni
{
namespace common
{
namespace event
{
template < typename D >
class Event : public IEvent
{
public:
    explicit Event( const D& data = D( ) )
        : m_data{ data }
    {
    }

    static void
    broadcast_event( const D& data, IEventSender& sender )
    {
        auto event = std::make_shared< const Event< D > >( Event< D >( data ) );
        sender.send( event );
    }

    const D&
    get_data( ) const noexcept
    {
        return m_data;
    }

private:
    /// Implementation of interface IEvent
    size_t
    get_type_id( ) const override
    {
        return helpers::type_id< D >( );
    }

    const std::string&
    get_type_name( ) const override
    {
        return helpers::type_name< D >( );
    }

private:
    D m_data;
};

}  // namespace event
}  // namespace common
}  // namespace uni

#define DECLARE_EVENT_AND_DATA_1( Name, type1, name1 )       \
    struct Name                                              \
    {                                                        \
        type1 name1;                                         \
    };                                                       \
    using Name##Event = ::uni::common::event::Event< Name >; \
    using Name##Listener = ::uni::common::event::EventListener< Name >
