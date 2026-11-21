# ECS is header-only and needs neither DirectX nor the game window.
add_executable(CrashOutEcsTests EcsTests.cpp)
target_include_directories(CrashOutEcsTests PRIVATE "${PROJECT_SOURCE_DIR}/CrashOut")

# Physics tests link the engine library but do not create an Application or window.
add_executable(CrashOutPhysicsTests PhysicsTests.cpp)
target_link_libraries(CrashOutPhysicsTests PRIVATE CrashOut)

add_test(NAME ECS.CreateDestroy   COMMAND CrashOutEcsTests entities)
add_test(NAME ECS.Components      COMMAND CrashOutEcsTests components)
add_test(NAME ECS.RemoveCallbacks COMMAND CrashOutEcsTests callbacks)
add_test(NAME Physics.DestroyEntity   COMMAND CrashOutPhysicsTests destroy-entity)
add_test(NAME Physics.RemoveComponents COMMAND CrashOutPhysicsTests remove-components)
add_test(NAME Physics.StressCreateDestroy COMMAND CrashOutPhysicsTests stress)

set_tests_properties(
    ECS.CreateDestroy ECS.Components ECS.RemoveCallbacks
    Physics.DestroyEntity Physics.RemoveComponents Physics.StressCreateDestroy
    PROPERTIES TIMEOUT 30
)
