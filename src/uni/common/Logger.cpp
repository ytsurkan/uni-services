#include "uni/common/Logger.hpp"

namespace uni
{
namespace common
{
Logger&
get_logger( )
{
    static Logger logger;
    return logger;
}

const char*
to_string( LogLevel level )
{
    switch ( level )
    {
    case LogLevel::DEBUG:
        return "Debug";
    case LogLevel::ERROR:
        return "Error";
    case LogLevel::INFO:
        return "Info";
    case LogLevel::WARNING:
        return "Warning";
    }
    return "Undefined";
}

}  // namespace common
}  // namespace uni
