# Gather all source and header files recursively (or list them manually)
file(GLOB_RECURSE CRASHOUT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Build the engine as a static library
add_library(CrashOut STATIC ${CRASHOUT_SOURCES})

# Tell other projects where to find your engine's headers
target_include_directories(CrashOut PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)