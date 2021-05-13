#pragma once

#include <memory>

#include "uni/common/event/FwdDeclarations.hpp"
#include "uni/common/event/IEventSender.hpp"

namespace uni
{
namespace common
{
namespace event
{
class EventSender : public IEventSender
{
public:
    explicit EventSender( std::weak_ptr< IEventReceiver > event_receiver )
        : m_event_receiver{ std::move( event_receiver ) }
    {
    }

private:
    void send( std::shared_ptr< const IEvent > event ) override;

private:
    std::weak_ptr< IEventReceiver > m_event_receiver;
};

}  // namespace event
}  // namespace common
}  // namespace uni
