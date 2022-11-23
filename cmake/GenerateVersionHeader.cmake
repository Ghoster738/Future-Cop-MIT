# Project Versioning with CMake and Git
# From https://github.com/nocnokneo/cmake-git-versioning-example

# Generate a "git describe" version string from Git repository tags
# https://git-scm.com/docs/git-describe
if(GIT_EXECUTABLE)

    # Find the version
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
        RESULT_VARIABLE GIT_DESCRIBE_ERROR_CODE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    # Use the found version if no errors occurred
    if(NOT GIT_DESCRIBE_ERROR_CODE)
        set(FUTURE_COP_MIT_VERSION_FOUND ${GIT_DESCRIBE_VERSION})
    endif()
endif()

# Fallback: Just use a bogus version string that is semantically older
# than anything else and spit out a warning to the developer.
if(NOT DEFINED FUTURE_COP_MIT_VERSION_FOUND)
    set(FUTURE_COP_MIT_VERSION_FOUND "v0.0.0-unknown")
    message(NOTICE "Failed to determine project version from Git - using default version \"${FUTURE_COP_MIT_VERSION_FOUND}\"")
endif()
