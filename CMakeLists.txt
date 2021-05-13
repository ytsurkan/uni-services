cmake_minimum_required( VERSION 3.11 )

project( uni-services )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(bootstrap)

uni_cmake_set_cxxstd(17)

set( SOURCE_DIR "${CMAKE_SOURCE_DIR}" )
set( BUILD_DIR "${CMAKE_BINARY_DIR}" )
set( INSTALL_DIR "${BUILD_DIR}/install" )

set( CMAKE_INSTALL_BINDIR bin )
set( CMAKE_INSTALL_DOCDIR doc )
set( CMAKE_INSTALL_INCLUDEDIR include )
set( CMAKE_INSTALL_LIBDIR lib )

set( CMAKE_INSTALL_PREFIX ${INSTALL_DIR} )
set( CMAKE_INSTALL_RPATH ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR} )

set( HEADERS
    "public/uni/common/event/IDispatcher.hpp"
    "public/uni/common/event/IEvent.hpp"
    "public/uni/common/event/Event.hpp"
    "public/uni/common/event/IEventSender.hpp"
    "public/uni/common/event/IEventReceiver.hpp"
    "public/uni/common/event/EventListener.hpp"
    "public/uni/common/event/EventThread.hpp"
    "public/uni/common/event/Factory.hpp"
    "public/uni/common/event/FwdDeclarations.hpp"
    "public/uni/common/event/Helpers.hpp"
    "public/uni/common/service/Broadcast.hpp"
    "public/uni/common/service/Service.hpp"
    "public/uni/common/concurrency/Queue.hpp"
    "public/uni/common/concurrency/ThreadPool.hpp"
    "public/uni/common/Logger.hpp"
)

set( SOURCES
    "src/uni/common/event/Dispatcher.hpp"
    "src/uni/common/event/Dispatcher.cpp"
    "src/uni/common/event/Factory.cpp"
    "src/uni/common/event/EventReceiver.hpp"
    "src/uni/common/event/EventReceiver.cpp"
    "src/uni/common/event/EventSender.hpp"
    "src/uni/common/event/EventSender.cpp"
    "src/uni/common/Logger.cpp"
)

add_library( ${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC
        ${SOURCE_DIR}/public
    PRIVATE
        ${SOURCE_DIR}/src
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_subdirectory( ${SOURCE_DIR}/external/googletest )
    add_subdirectory( tests )
endif()


install( TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# The trailing slash is important
install(DIRECTORY ${SOURCE_DIR}/public/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)
