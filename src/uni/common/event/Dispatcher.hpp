#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include "uni/common/event/IDispatcher.hpp"

namespace uni
{
namespace common
{
namespace event
{
class IEventListener;
class IEvent;

class Dispatcher : public IDispatcher
{
private:
    void dispatch( const IEvent& event ) override;
    void register_listener( IEventListener* listener ) override;
    void unregister_listener( IEventListener* listener ) override;

private:
    using Listeners = std::vector< IEventListener* >;
    using TypeId = size_t;

    bool get_listeners( size_t type_id, Listeners& listeners );

private:
    std::unordered_map< TypeId, Listeners > m_listeners{ };
    std::mutex m_mutex{ };
};

}  // namespace event
}  // namespace common
}  // namespace uni
