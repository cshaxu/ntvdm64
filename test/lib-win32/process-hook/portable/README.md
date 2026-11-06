# Portable process-hook tests

Configure this directory with a Windows x86 or x64 C17 compiler and Ninja.
For MSVC, first run the matching `vcvarsall.bat x86` or `vcvarsall.bat x64`.
Then run:

```
cmake -S <this-directory> -B build/.ninja/process-hook-<compiler>-<arch> -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/.ninja/process-hook-<compiler>-<arch>
ctest --test-dir build/.ninja/process-hook-<compiler>-<arch> --output-on-failure
```

Use separate build directories for each compiler and target architecture.
MinGW must be selected using the corresponding C compiler/toolchain. These
self-contained tests exercise the current project sources and create only their
own child processes. They do not validate a consuming product's policy,
cross-architecture deployment or compatibility with independently changed
versions of this library. Installed late trampolines are retained, not removed.

The source directory may live under either test/ or tests/. Fixture paths are
relative to this file; no consuming executable-private headers are required.
Consumers may add this directory to their configured build to retain their
own cache/compiler policy rather than use the standalone invocation.
