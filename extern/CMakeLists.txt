find_library(AVCODEC_LIBRARY avcodec REQUIRED)
find_library(AVFORMAT_LIBRARY avformat REQUIRED)
find_library(AVDEVICE_LIBRARY avdevice REQUIRED)
find_library(AVFILTER_LIBRARY avfilter REQUIRED)
find_library(AVUTIL_LIBRARY avutil REQUIRED)
find_library(SWSCALE_LIBRARY swscale REQUIRED)
find_library(SWRESAMPLE_LIBRARY swresample REQUIRED)

option(GLFW_INSTALL OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory("${CMAKE_SOURCE_DIR}/extern/glfw/")

### Export to parent scope ###

set(GLAD_SOURCE
    "${CMAKE_SOURCE_DIR}/extern/glad/src/glad.c"
    PARENT_SCOPE
    )

set(EXTERN_INCLUDES
    "${CMAKE_SOURCE_DIR}/extern/glad/include/"
    "${CMAKE_SOURCE_DIR}/extern/glfw/include/"
    PARENT_SCOPE
    )

set(EXTERN_LIBRARIES
    glfw
    ${GLAD_LIBRARIES}
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVDEVICE_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWSCALE_LIBRARY}
    ${SWRESAMPLE_LIBRARY}
    PARENT_SCOPE
    )

