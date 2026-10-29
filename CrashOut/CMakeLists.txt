file(GLOB_RECURSE CRASHOUT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(CrashOut STATIC ${CRASHOUT_SOURCES})

target_include_directories(CrashOut PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)