#pragma once

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <set>

namespace uni
{
namespace common
{
namespace concurrency
{
struct QueueClosedException : public std::exception
{
    const char*
    what( ) const noexcept override
    {
        return "QueueIsClosed";
    }
};

enum class OperationStatus
{
    success,
    closed
};


template < typename T = std::function< void( ) >, typename Container = std::deque< T > >
class Queue
{
public:
    Queue( ) = default;

    Queue( const Queue& ) = delete;
    Queue& operator=( const Queue& ) = delete;

    Queue( Queue&& ) = delete;
    Queue& operator=( Queue&& ) = delete;

    ~Queue( ) = default;

    void
    push( T& element )
    {
        std::lock_guard< std::mutex > guard( m_mutex );
        throw_if_closed( );
        m_elements.push_back( element );
        m_cond.notify_one( );
    }

    void
    push( T&& element )
    {
        std::lock_guard< std::mutex > guard( m_mutex );
        throw_if_closed( );
        m_elements.push_back( std::move( element ) );
        m_cond.notify_one( );
    }

    OperationStatus
    wait_pop( T& element )
    {
        std::unique_lock< std::mutex > lock( m_mutex );
        m_cond.wait( lock, [this]( ) { return is_not_empty_or_closed( ); } );
        if ( empty( lock ) )
        {
            return OperationStatus::closed;
        }
        element = std::move( m_elements.front( ) );
        m_elements.pop_front( );
        return OperationStatus::success;
    }

    ///@brief After calling close, queue doesn't wait for new elements and fails on submission of
    /// new elements.
    void
    close( )
    {
        {
            std::lock_guard< std::mutex > guard( m_mutex );
            m_closed = true;
        }
        m_cond.notify_all( );
    }

    bool
    closed( ) const
    {
        std::lock_guard< std::mutex > guard( m_mutex );
        return m_closed;
    }

    bool
    empty( ) const
    {
        std::lock_guard< std::mutex > guard( m_mutex );
        return m_elements.empty( );
    }

    template < typename Predicate >
    bool
    remove( Predicate&& predicate )
    {
        std::lock_guard< std::mutex > guard( m_mutex );
        const auto pos = m_elements.erase( std::remove_if( std::begin( m_elements ),
                                                           std::end( m_elements ),
                                                           std::forward< Predicate >( predicate ) ),
                                           std::end( m_elements ) );
        return ( pos != std::end( m_elements ) );
    }

private:
    bool
    empty( std::unique_lock< std::mutex >& ) const noexcept
    {
        return m_elements.empty( );
    }

    bool
    is_not_empty_or_closed( ) const noexcept
    {
        return !m_elements.empty( ) || m_closed;
    }

    void
    throw_if_closed( ) const
    {
        if ( m_closed )
        {
            throw QueueClosedException( );
        }
    }

private:
    Container m_elements{};
    mutable std::mutex m_mutex{};
    std::condition_variable m_cond{};
    bool m_closed{false};
};

}  // namespace concurrency
}  // namespace common
}  // namespace uni
