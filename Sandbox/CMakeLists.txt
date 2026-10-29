add_executable(Sandbox 
    Src/Application.cpp
    Src/Application.h
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