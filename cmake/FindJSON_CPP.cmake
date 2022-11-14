# Thanks ecrafter!
# This is probably based on the MYGUI cmake file.

set (JSON_CPP_ROOT_DIR "./")
# Uncomment the following line to print which directory CMake is looking in.
# MESSAGE(STATUS "JSON_CPP_ROOT_DIR: " ${JSON_CPP_ROOT_DIR})

find_path( JSON_CPP_INCLUDE_DIR
    NAMES json/json.h
    PATH_SUFFIXES include jsoncpp include/jsoncpp
    PATHS ${JSON_CPP_ROOT_DIR}
    DOC "The JSON_CPP include directory"
)

find_library( JSON_CPP_LIBRARY 
    NAMES jsoncpp
    PATHS ${JSON_CPP_ROOT_DIR}
    DOC "The JSON_CPP library"
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LOGGING_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args( JSON_CPP DEFAULT_MSG JSON_CPP_INCLUDE_DIR JSON_CPP_LIBRARY)

# Tell cmake GUIs to hide the "local" variables if found
if(JSON_CPP_FOUND)
    mark_as_advanced( JSON_CPP_INCLUDE_DIR JSON_CPP_LIBRARY )
endif()
