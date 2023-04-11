###############################################################################
##-------------------------------SOURCE-FILES--------------------------------##
###############################################################################

# files at root will be added to executable automatically like main
file(GLOB MAIN "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

list(APPEND SOURCE_DIRS
    # place sub-dirs that will be added to executable
    "core"
    "render"
    )

list(APPEND CMAKER_IGNORE
    # place ignored dirs (usually for libs)
    )

###############################################################################
##--------------------------------END-SOURCE---------------------------------##
###############################################################################



###############################################################################
##---------------------------------ADD-LIBS----------------------------------##
###############################################################################
#
# declare internal libs here after placing
# them in CMAKER_IGNORE
#
###############################################################################
##---------------------------------END-LIBS----------------------------------##
###############################################################################



###############################################################################
##-------------------------------BUILD-CHECKS--------------------------------##
###############################################################################
file(GLOB ROOT "${CMAKE_CURRENT_SOURCE_DIR}/*")

# Get FDs in ROOT
list(APPEND PRESENT_DIRS)
foreach(FD ${ROOT})
    if(IS_DIRECTORY ${FD})
        get_filename_component(DIR_NAME ${FD} NAME)
        list(APPEND PRESENT_DIRS ${DIR_NAME})
    endif()
endforeach()

# Check subdirs for c/cpp files
function(RECURSE_DIRS FD LS)
    file(GLOB_RECURSE CPP "${CMAKE_CURRENT_SOURCE_DIR}/${FD}/*.cpp")
    file(GLOB_RECURSE CXX "${CMAKE_CURRENT_SOURCE_DIR}/${FD}/*.cxx")
    file(GLOB_RECURSE CC "${CMAKE_CURRENT_SOURCE_DIR}/${FD}/*.cc")
    file(GLOB_RECURSE C "${CMAKE_CURRENT_SOURCE_DIR}/${FD}/*.c")
    list(APPEND RET ${CPP} ${CXX} ${CC} ${C})
    # CMake return statement (push to parent scope)
    set(${LS} ${RET} PARENT_SCOPE)
endfunction()

# Warn if there are unincluded c/cpp files in src
foreach(FD ${PRESENT_DIRS})
    if(NOT ${FD} IN_LIST SOURCE_DIRS AND NOT ${FD} IN_LIST CMAKER_IGNORE)
        RECURSE_DIRS(${FD} CHECK_SOURCES)
        foreach(FILE ${CHECK_SOURCES})
            message(AUTHOR_WARNING
                "-> " ${FILE} " is NOT added to CMakeLists.txt")
        endforeach()
    endif()
endforeach()
###############################################################################
##--------------------------------END-CHECKS---------------------------------##
###############################################################################



###############################################################################
##----------------------------------EXPORT-----------------------------------##
###############################################################################
foreach(DIR ${SOURCE_DIRS})
    RECURSE_DIRS(${DIR} SOURCES)
endforeach()

set(APP_SOURCE
    ${MAIN}
    ${SOURCES}
    PARENT_SCOPE
    )

set(APP_INCLUDE
    "${CMAKE_SOURCE_DIR}/src/"
    PARENT_SCOPE
    )
###############################################################################
##--------------------------------END-EXPORT---------------------------------##
###############################################################################

