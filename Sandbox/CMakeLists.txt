add_executable(Sandbox 
    Src/Game.cpp
    Src/Game.h
)

target_link_libraries(Sandbox PRIVATE CrashOut)

target_include_directories(Sandbox PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../CrashOut
)


# Tell Visual Studio's debugger where to find the executable and set its working directory
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_COMMAND "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/Sandbox.exe"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>"
)

# Copy game assets next to Sandbox.exe
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
        "$<TARGET_FILE_DIR:Sandbox>/Assets"

    # Copy engine shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/CrashOut/Shaders"
        "$<TARGET_FILE_DIR:Sandbox>/Shaders"

    VERBATIM
)