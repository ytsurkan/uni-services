#pragma once

#include "uni/common/event/EventThread.hpp"
#include "uni/common/event/FwdDeclarations.hpp"
#include "uni/common/event/IEventReceiver.hpp"

namespace uni
{
namespace common
{
namespace event
{
class EventReceiver : public IEventReceiver
{
public:
    explicit EventReceiver( IDispatcher& dispatcher, EventThread& thread );

private:
    void receive( std::shared_ptr< const IEvent > event ) override;

private:
    IDispatcher& m_dispatcher;
    EventThread& m_thread;
};
}  // namespace event
}  // namespace common
}  // namespace uni
