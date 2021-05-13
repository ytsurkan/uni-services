#pragma once

#include "uni/common/concurrency/ThreadPool.hpp"

namespace uni
{
namespace common
{
namespace event
{
using EventThread = ::uni::common::concurrency::ThreadPool< std::function< void( ) > >;
}  // namespace event
}  // namespace common
}  // namespace uni
