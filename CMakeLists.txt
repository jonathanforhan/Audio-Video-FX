cmake_minimum_required(VERSION 3.20)

project(avfx)

# see cmaker-config.json for additional config flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")
endif()

# set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)

add_subdirectory(extern)
add_subdirectory(src)

add_executable(${CMAKE_PROJECT_NAME}
    ${APP_SOURCE}
    ${GLAD_SOURCE}
    )

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${EXTERN_LIBRARIES}
    Qt6::Widgets
    Qt6::OpenGLWidgets
    )

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
    ${EXTERN_INCLUDES}
    ${APP_INCLUDE}
    )

