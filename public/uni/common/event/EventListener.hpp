#pragma once

#include "uni/common/event/Event.hpp"
#include "uni/common/event/Helpers.hpp"
#include "uni/common/event/IDispatcher.hpp"

namespace uni
{
namespace common
{
namespace event
{
class IEventListener
{
public:
    IEventListener( ) = default;
    IEventListener( const IEventListener& ) = delete;
    IEventListener& operator=( const IEventListener& ) = delete;
    IEventListener( IEventListener&& ) = default;
    IEventListener& operator=( IEventListener&& ) = default;
    virtual ~IEventListener( ) = default;

public:
    virtual void process_event( const IEvent& event ) = 0;
    virtual size_t get_type_id( ) const = 0;
    virtual const std::string& get_type_name( ) const = 0;
};

template < typename D >
class TEventListener : public IEventListener
{
protected:
    /// Implementation of interface IEventListener
    void
    process_event( const IEvent& event ) override
    {
        if ( event.get_type_id( ) == get_type_id( ) )
        {
            process_event( static_cast< const Event< D >& >( event ) );
        }
    }

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

protected:
    void
    register_listener( IDispatcher& dispatcher )
    {
        dispatcher.register_listener( this );
        m_dispatcher = &dispatcher;
    }

    void
    unregister_listener( )
    {
        if ( nullptr != m_dispatcher )
        {
            m_dispatcher->unregister_listener( this );
            m_dispatcher = nullptr;
        }
    }

protected:
    virtual void process_event( const Event< D >& event ) = 0;

private:
    IDispatcher* m_dispatcher{ nullptr };
};

template < typename D >
class EventListener : public TEventListener< D >
{
    using Base = TEventListener< D >;

public:
    explicit EventListener( IDispatcher& dispatcher )
    {
        Base::register_listener( dispatcher );
    }

    ~EventListener( )
    {
        Base::unregister_listener( );
    }
};
}  // namespace event
}  // namespace common
}  // namespace uni
