cmake_minimum_required(VERSION 3.16)
project(CinemaAutoatendimento LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

find_package(Qt6 COMPONENTS Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
set(RESOURCES "src/recursos.qrc") 

add_executable(CinemaAutoatendimento ${SOURCES} ${HEADERS} ${RESOURCES})
if(WIN32)
    target_link_libraries(CinemaAutoatendimento PRIVATE dwmapi)
endif()
if(Qt6_FOUND)
    target_link_libraries(CinemaAutoatendimento PRIVATE Qt6::Widgets)
else()
    target_link_libraries(CinemaAutoatendimento PRIVATE Qt5::Widgets)
endif()