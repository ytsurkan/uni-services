set( CMAKE_THREAD_PREFER_PTHREAD TRUE )

find_package( Threads REQUIRED )
if (DEFINED CMAKE_THREAD_LIBS_INIT)
    message( STATUS "Thread library - ${CMAKE_THREAD_LIBS_INIT}" )
else()
    message( FATAL_ERROR "Thread library - not found" )
endif()
