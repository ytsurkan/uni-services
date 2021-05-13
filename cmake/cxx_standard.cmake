
macro(uni_cmake_set_cxxstd cxx)
    set(CMAKE_CXX_STANDARD "${cxx}")
    set(CMAKE_CXX_STANDARD_REQUIRED on)
    set(CMAKE_CXX_EXTENSIONS OFF)

    message( STATUS "C++${CMAKE_CXX_STANDARD} standard is enabled" )
endmacro()
