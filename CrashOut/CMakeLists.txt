
file(GLOB_RECURSE CRASHOUT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Exclude external libraries to prevent double compilation
list(FILTER CRASHOUT_SOURCES EXCLUDE REGEX "/External/box3d-main/")
list(FILTER CRASHOUT_SOURCES EXCLUDE REGEX "/External/DirectXTK/")

# Create engine library
add_library(CrashOut STATIC ${CRASHOUT_SOURCES})

# Box3D
add_subdirectory(External/box3d-main)

# DirectXTK configuration
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_XAUDIO_WIN8 OFF CACHE BOOL "" FORCE)
set(BUILD_XAUDIO_WIN10 OFF CACHE BOOL "" FORCE)
set(BUILD_XAUDIO_REDIST OFF CACHE BOOL "" FORCE)

# DirectXTK
add_subdirectory(External/DirectXTK)

# Include directories
target_include_directories(CrashOut PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(CrashOut PUBLIC
    box3d::box3d
    DirectXTK
    d3d11
    d3dcompiler
    dxgi
)
