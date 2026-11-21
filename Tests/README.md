# CrashOut automated tests

From a **Visual Studio 2022 Developer PowerShell** at the repository root:

```powershell
cmake --preset win-vs2022
cmake --build --preset win-debug-tests
ctest --preset win-debug-tests
```

To validate a truly clean build, delete `build/windows-vs2022` and rerun the
three commands. You can also use `ctest --preset win-debug-tests -V` to print
individual test output.

The tests run without creating the Sandbox window. They require Windows/MSVC
and the existing DirectX dependencies to compile `CrashOut`. The
`EcsTests.cpp` file can also be compiled standalone on other platforms for
focused ECS verification.

`RegisterPhysicsCleanupCallbacks` is intentionally shared between the application
and the tests, so regression tests exercise the actual cleanup wiring.
