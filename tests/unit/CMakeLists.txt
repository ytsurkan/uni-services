project( uni-services-unittests )

set( HEADERS
)

set( SOURCES
    "uni/common/EventTest.cpp"
    "uni/common/BroadcastTest.cpp"
    "uni/common/ServiceTest.cpp"
    "uni/common/DemoApplicationTest.cpp"
)

add_executable( ${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
)

target_include_directories( ${PROJECT_NAME}
    PRIVATE
        ${SOURCE_DIR}/uni-services/public
    PRIVATE
        ${SOURCE_DIR}/external/googletest/
)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        uni-services
        gtest
        gtest_main
        gmock
)

cmake_print_properties(TARGETS ${PROJECT_NAME} PROPERTIES INSTALL_RPATH)
