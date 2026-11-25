
# ============================================================
# Editor source files
# ============================================================

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Exclude ImGui from the Editor source collection
list(FILTER EDITOR_SOURCES EXCLUDE REGEX "/External/ImGui/")

# ============================================================
# ImGui
# ============================================================

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/ImGui")

add_library(ImGui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

target_include_directories(ImGui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ============================================================
# ImGui backends
# ============================================================

# Example: Win32 + DirectX 11
# Uncomment if CrashOut uses these APIs.

 target_sources(ImGui PRIVATE
     ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
     ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
 )

 target_link_libraries(ImGui PRIVATE
     d3d11
     d3dcompiler
     dwmapi
     gdi32
 )

# ============================================================
# Editor executable
# ============================================================

add_executable(Editor
    ${EDITOR_SOURCES}
)

target_link_libraries(Editor PRIVATE
    CrashOut
    ImGui
)

target_include_directories(Editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../CrashOut
)

# ============================================================
# Visual Studio debugger settings
# ============================================================

set_target_properties(Editor PROPERTIES
    VS_DEBUGGER_COMMAND "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/Editor.exe"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

# ============================================================
# Copy shaders and assets
# ============================================================

add_custom_command(TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/CrashOut/Shaders"
        "$<TARGET_FILE_DIR:Editor>/Shaders"

    # Optional: copy shared/sample assets
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     "${CMAKE_CURRENT_SOURCE_DIR}/../Sandbox/Assets"
    #     "$<TARGET_FILE_DIR:Editor>/Assets"

    # Optional: copy scenes
    # COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     "${CMAKE_CURRENT_SOURCE_DIR}/../Sandbox/Scenes"
    #     "$<TARGET_FILE_DIR:Editor>/Scenes"

    VERBATIM
)
