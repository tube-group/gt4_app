# gt4_app Agent Notes

## Scope

- This file applies to the gt4_app repository.
- The VS Code workspace also contains gt4_app_lzy and gt4_app_wangy. Do not copy, compare, or sync edits across those sibling repos unless the user explicitly asks for it.

## Build And Validation

- This is a Linux-first C++20 CMake project. The root [CMakePresets.json](CMakePresets.json) only defines the linux-debug preset and expects the vcpkg toolchain at /app/vcpkg/scripts/buildsystems/vcpkg.cmake.
- Standard build commands from the repository root are:

  ```bash
  cmake --preset linux-debug
  cmake --build build -j
  ```

- All executables are emitted under the bin directory by the root [CMakeLists.txt](CMakeLists.txt).
- The repository does not include an automated test suite. After changes, prefer the narrowest possible validation: configure or build the touched target on Linux. If the current environment cannot run that validation, state that clearly instead of implying success.

## Project Map

- The root [CMakeLists.txt](CMakeLists.txt) builds seven subprojects: CommL3, TubeTrack, FormDemo, MonitorUserCmd, MonitorPlcData, SprayWeight, and TagPrint.
- [TubeTrack/src/workthread.cpp](TubeTrack/src/workthread.cpp) is the main steel-tube flow orchestration entry point. Shared runtime state is concentrated in [TubeTrack/include/TubeTrackContext.h](TubeTrack/include/TubeTrackContext.h).
- CommL3 handles Gauss or PostgreSQL-facing communication.
- MonitorUserCmd and MonitorPlcData are Redis-driven listener processes.
- SprayWeight and TagPrint are downstream operational modules. FormDemo is a demo subscriber.
- Cross-module command and event payloads live in [include/usercmd.h](include/usercmd.h). Shared utility headers live under [include](include).

## Repository Conventions

- Runtime configuration is kept in module-specific files under [config](config). When changing a config key, update the matching parser or usage in the same change.
- Treat values in the INI files as deployment data. Do not normalize or rotate host, account, or password settings unless the user asked for that specific change.
- Third-party dependencies are resolved mostly through find_package. [TubeTrack/CMakeLists.txt](TubeTrack/CMakeLists.txt) also requires higplat from /usr/local/lib.
- The root [README.md](README.md) is intentionally minimal. When documenting behavior, link to the owning source file, config file, or docs folder instead of duplicating details here.

## Working Guidance

- Start from the module that directly owns the behavior instead of changing several executables at once.
- For TubeTrack work, preserve the existing ownership and handoff model around ProductionPlan::Pop() and downstream Push() calls.
- Do not assume Windows build parity from the preset configuration; Windows sessions often need manual CMake configuration.
- For database or gplat related work, use [doc/database](doc/database) and [doc/gplat](doc/gplat) as the first references and keep this file high level.