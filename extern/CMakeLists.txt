find_library(AVCODEC_LIBRARY avcodec REQUIRED)
find_library(AVFORMAT_LIBRARY avformat REQUIRED)
find_library(AVDEVICE_LIBRARY avdevice REQUIRED)
find_library(AVFILTER_LIBRARY avfilter REQUIRED)
find_library(AVUTIL_LIBRARY avutil REQUIRED)
find_library(SWSCALE_LIBRARY swscale REQUIRED)
find_library(SWRESAMPLE_LIBRARY swresample REQUIRED)

macro(EXTERN_SETUP)
    # QT's meta-object-compiler
    set(CMAKE_AUTOMOC ON)
    find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)
endmacro()

### Export to parent scope ###

set(EXTERN_LIBRARIES
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVDEVICE_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWSCALE_LIBRARY}
    ${SWRESAMPLE_LIBRARY}
    Qt::Widgets
    Qt::OpenGLWidgets
    PARENT_SCOPE
    )

