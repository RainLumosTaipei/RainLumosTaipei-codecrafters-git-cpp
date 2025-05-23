cmake_minimum_required(VERSION 3.13)

project(codecrafters-git)

set(CMAKE_CXX_STANDARD 17) # Enable the C++23 standard

file(GLOB_RECURSE SOURCE_FILES src/*.cpp include/*.h)

if (WIN32)
else ()
    find_package(OpenSSL REQUIRED)
    find_package(ZLIB REQUIRED)
endif ()

add_executable(git ${SOURCE_FILES})

target_include_directories(git PUBLIC include)

if (WIN32)
    target_link_libraries(git PRIVATE)
else ()
    target_link_libraries(git PRIVATE OpenSSL::Crypto)
    target_link_libraries(git PRIVATE ZLIB::ZLIB)
endif ()