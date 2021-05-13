#pragma once

#include <atomic>
#include <thread>
#include <uni/common/concurrency/Queue.hpp>
#include <uni/common/event/IEvent.hpp>

#include "uni/common/Logger.hpp"

namespace uni
{
namespace common
{
namespace concurrency
{
struct ThreadPoolStoppedException : public std::exception
{
    const char*
    what( ) const noexcept override
    {
        return "QueueIsStopped";
    }
};

template < typename TaskType >
class ThreadPool
{
public:
    explicit ThreadPool( size_t threads_count = 1 )
        : m_threads_count{ threads_count }
    {
    }

    ThreadPool( const ThreadPool& ) = delete;
    ThreadPool& operator=( const ThreadPool& ) = delete;

    ThreadPool( ThreadPool&& ) = delete;
    ThreadPool& operator=( ThreadPool&& ) = delete;

    virtual ~ThreadPool( )
    {
        do_stop( );
        if ( !m_queue.empty( ) )
        {
            LOG_ERROR( "Unexpected tasks left in queue!" );
        }
    }

    void
    start( )
    {
        if ( m_started )
        {
            throw std::logic_error( "ThreadPool already started!" );
        }
        m_started = true;
        do_start( );
    }

    /**
     * @brief Executes all tasks that were already added to thread pool at the moment of execution
     * of stop_impl member function.
     */
    void
    stop( )
    {
        if ( !m_started )
        {
            throw std::logic_error( "ThreadPool already stopped!" );
        }
        m_started = false;
        do_stop( );
    }

    template < typename Task >
    void
    add_task( Task&& task )
    {
        throw_if_stopped( );
        m_queue.push( std::forward< Task >( task ) );
    }

    bool
    on_one_of_threads( ) const
    {
        for ( const auto& thread : m_threads )
        {
            if ( std::this_thread::get_id( ) == thread.get_id( ) )
            {
                return true;
            }
        }
        return false;
    }

private:
    void
    worker_thread( )
    {
        while ( true )
        {
            try
            {
                TaskType task;
                if ( OperationStatus::closed == m_queue.wait_pop( task ) )
                {
                    return;
                }
                task( );
            }
            catch ( const std::exception& error )
            {
                LOG_ERROR( "Task execution failed with error: ", error.what( ) );
            }
        }
    }

    void
    close( )
    {
        m_queue.close( );
    }

    void
    join( )
    {
        for ( auto& thread : m_threads )
        {
            if ( thread.joinable( ) )
            {
                try
                {
                    thread.join( );
                }
                catch ( const std::system_error& error )
                {
                    LOG_ERROR( "Thread join failed with error: ", error.what( ) );
                }
            }
        }
    }

    void
    do_start( )
    {
        m_threads.reserve( m_threads_count );
        for ( size_t i = 0; i < m_threads_count; ++i )
        {
            std::thread thread( [ this ]( ) { worker_thread( ); } );
            m_threads.push_back( std::move( thread ) );
        }
    }

    void
    do_stop( )
    {
        close( );
        join( );
    }

    void
    throw_if_stopped( )
    {
        if ( !m_started )
        {
            throw ThreadPoolStoppedException( );
        }
    }

private:
    Queue< TaskType > m_queue{ };
    const size_t m_threads_count{ 0 };
    std::atomic< bool > m_started{ false };
    std::vector< std::thread > m_threads{ };
};

}  // namespace concurrency
}  // namespace common
}  // namespace uni
