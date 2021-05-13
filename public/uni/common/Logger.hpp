#pragma once

#include <iostream>
#include <mutex>

namespace uni
{
namespace common
{
enum class LogLevel
{
    ERROR,
    WARNING,
    INFO,
    DEBUG,
};
const char* to_string( LogLevel level );

class Logger
{
public:
    template < typename... Args >
    void
    log_message(
        LogLevel level, const char* file, uint32_t line, const char* function, Args&&... args )
    {
        std::lock_guard< std::mutex > lock( m_mutex );
        m_logger << '[' << to_string( level ) << ']' << file << ':' << line << ' ';
        m_logger << function << ": ";

        // For C++11 pack expansion on comma operator expression, can be used instead of C++17 fold
        // expressions.
        //        const int32_t _[] = { 0, ( m_logger << std::forward< Args >( args ), 0 )... };
        //        static_cast< void >( _ );
        //        m_logger << '\n';
        ( m_logger << ... << args ) << '\n';
    }

private:
private:
    std::mutex m_mutex{ };
    std::ostream& m_logger{ std::cout };
};

Logger& get_logger( );

}  // namespace common
}  // namespace uni

#define LOG_MESSAGE( level, file, line, function, ... ) \
    ::uni::common::get_logger( ).log_message( level, file, line, function, __VA_ARGS__ );

#define LOG_ERROR( ... ) \
    LOG_MESSAGE( ::uni::common::LogLevel::ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#define LOG_WARNING( ... ) \
    LOG_MESSAGE( ::uni::common::LogLevel::WARNING, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#define LOG_INFO( ... ) \
    LOG_MESSAGE( ::uni::common::LogLevel::INFO, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#define LOG_DEBUG( ... ) \
    LOG_MESSAGE( ::uni::common::LogLevel::DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

template < typename T >
const char*
to_key( const T&, const char* key )
{
    return key;
}

inline const char*
to_key( const char* key, const char* )
{
    return key;
}

template < typename T >
const T&
to_value( const T& value )
{
    return value;
}

template < typename T >
const T&
to_value( const char*, const T& value )
{
    return value;
}

template < typename Key, typename Value >
void
to_json_impl( std::ostream& out, const Key& key, const Value& value )
{
    out << key << ":" << value;
}

template < typename Key, typename Value, typename... Args >
void
to_json_impl( std::ostream& out, const Key& key, const Value& value, Args&&... args )
{
    out << key << ":" << value;
    out << ",";
    to_json_impl( out, std::forward< Args >( args )... );
}

template < typename... Args >
void
to_json( std::ostream& out, Args&&... args )
{
    out << "{";
    to_json_impl( out, std::forward< Args >( args )... );
    out << "}";
}
/**
 * int x = 42;
 * LOG_ITEM(x); will log value x in json format -> x:42
 * LOG_ITEM("index", x); will log value x in json format -> index:42
 */
#define LOG_ITEM( x, ... ) to_key( x, #x ), to_value( x, ##__VA_ARGS__ )

/**
 * struct Boo
 * {
 *   int n;
 *   std::string m;
 *   LOG_STRUCT( Boo, LOG_ITEM( n ), LOG_ITEM( m ) );
 * };
 * Can be used like this:
 * Boo boo;
 * LOG_DEBUG( LOG_ITEM( "boo", boo ) );
 * LOG_DEBUG( LOG_ITEM( boo ) );
 */
#define LOG_STRUCT( Struct, ... )                                                  \
    std::ostream& to_stream( std::ostream& out ) const                             \
    {                                                                              \
        to_json( out, __VA_ARGS__ );                                               \
        return out;                                                                \
    }                                                                              \
    friend inline std::ostream& operator<<( std::ostream& out, const Struct& obj ) \
    {                                                                              \
        return obj.to_stream( out );                                               \
    }
