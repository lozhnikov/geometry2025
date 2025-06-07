
cmake_minimum_required(VERSION 3.10)

# Настройка компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Явно отключаем проблемные флаги для GCC
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    remove_definitions(-D_LIBCPP_DEBUG=1)
    string(REGEX REPLACE "-stdlib=libc\\+\\+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

project(geometry LANGUAGES CXX)

# Настройка путей
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")
set(GEOMETRY_INCLUDE_DIRS "include")

# Загрузка зависимостей
include(FetchContent)

# Загрузка cpp-httplib
FetchContent_Declare(
    httplib
    URL https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.8.8.tar.gz
)
FetchContent_MakeAvailable(httplib)
set(GEOMETRY_INCLUDE_DIRS ${GEOMETRY_INCLUDE_DIRS} ${httplib_SOURCE_DIR})

# Загрузка nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.9.1/include.zip
)
FetchContent_MakeAvailable(nlohmann_json)
set(GEOMETRY_INCLUDE_DIRS ${GEOMETRY_INCLUDE_DIRS} ${nlohmann_json_SOURCE_DIR})

# Настройка библиотек
if(NOT MSVC)
    set(GEOMETRY_LIBS pthread)
endif()

if(WIN32)
    set(WINDOWS_LIBS ws2_32)
endif()

include_directories(${GEOMETRY_INCLUDE_DIRS})

# Серверная часть
add_executable(geometry_server
    include/alg_graham.hpp
    include/common.hpp
    include/edge.hpp
    include/edge_impl.hpp
    include/orientation.hpp
    include/point.hpp
    include/point_impl.hpp
    include/polygon.hpp
    include/polygon_impl.hpp
    methods/alg_graham_method.cpp
    methods/main.cpp
    methods/methods.hpp
)
target_link_libraries(geometry_server ${GEOMETRY_LIBS} ${WINDOWS_LIBS})

# Клиентская часть (тесты)
add_executable(geometry_test
    include/alg_graham.hpp
    include/common.hpp
    include/edge.hpp
    include/edge_impl.hpp
    include/orientation.hpp
    include/point.hpp
    include/point_impl.hpp
    include/polygon.hpp
    include/polygon_impl.hpp
    tests/alg_graham_test.cpp
    tests/edge_test.cpp
    tests/io.hpp
    tests/main.cpp
    tests/orientation_test.cpp
    tests/point_test.cpp
    tests/polygon_test.cpp
    tests/test.hpp
    tests/test_core.cpp
    tests/test_core.hpp
)
target_link_libraries(geometry_test ${GEOMETRY_LIBS} ${WINDOWS_LIBS})

# Установка политик CMake
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

