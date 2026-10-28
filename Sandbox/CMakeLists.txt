add_executable(Sandbox 
    Application.cpp
)

# Link your CrashOut engine library to the Sandbox
target_link_libraries(Sandbox PRIVATE CrashOut)