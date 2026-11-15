file(GLOB_RECURSE CRASHOUT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# CRITICAL: Exclude Box3D from your engine's source glob 
# so its files aren't compiled twice.
list(FILTER CRASHOUT_SOURCES EXCLUDE REGEX "/External/box3d-main/")

add_library(CrashOut STATIC ${CRASHOUT_SOURCES})

# 1. Add Box3D as a subdirectory from your External folder
add_subdirectory(External/box3d-main)

target_include_directories(CrashOut PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 2. Link box3d::box3d publicly so your Sandbox game project can also use it if needed
target_link_libraries(CrashOut PUBLIC
    box3d::box3d
    d3d11
    d3dcompiler
    dxgi
)