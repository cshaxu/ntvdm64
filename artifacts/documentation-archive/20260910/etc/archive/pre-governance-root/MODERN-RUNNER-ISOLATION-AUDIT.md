# Modern Runner Isolation Audit

Status: executed clean-build and smoke evidence, 2026-08-07. This audit covers
only the owned `ntdos64-run` dispatcher boundary. It does not demonstrate DOS
guest execution, an NTDOS bootstrap, or historical-host-stack recovery.

## Build Input

The current top-level `CMakeLists.txt` defines `ntdos64-run` from exactly one
source file:

```text
src/runner/ntdos64_run.c
```

The retired self-authored startup/BOP/DEM/IVT/BDA sources appear only in the
`NTDOS64_ENABLE_TRACE_INSTRUMENTATION`-guarded `ntvdm-startup-reference`
custom source target. That option was explicitly `OFF` for this audit. The
separate `src/opennt` and `refs/ntvdmx64-derived` CMake islands cannot be added to the
modern graph; their old top-level compatibility options fail configuration.

The archived `artifacts/build/legacy/referenced/build-root-pre-isolation/`
tree contains `nvtdm.exe`, `owned-dos-engine.exe`, or trace executables from
retired configurations. Those files are not authoritative build-graph evidence
and are not inputs to this clean build.

## Reproduction

A fresh directory was created at:

```text
toolchain-probe/default-runtime-isolation-build-v2
```

It used MSYS2 UCRT64 MinGW-w64 GCC 16.1.0 and CMake/Ninja, with:

```text
cmake -S <repo> -B <stage> -G Ninja \
  -DCMAKE_C_COMPILER=C:\\msys64\\ucrt64\\bin\\gcc.exe \
  -DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=OFF
cmake --build <stage> --target ntdos64-run --verbose
```

The verbose build compiled only `src/runner/ntdos64_run.c` and linked only the normal
Windows `shell32` dependency plus MinGW's standard runtime imports. No
historical source, trace source, driver, injected component, or guest file was
compiled or linked.

## Smoke Results

All executions used the freshly built executable with UCRT64 on `PATH`:

| Invocation | Result | Interpretation |
| --- | ---: | --- |
| no arguments | exit `2` | explicit usage rejection |
| `cmd.exe /c exit 17` | exit `17` | PE host handoff preserves child exit status |
| nonexistent `.com` without `--engine` | exit `3` | DOS/NE classification refuses to select a fallback engine |

The last check deliberately uses no guest executable payload. It proves only
that a DOS-shaped target does not activate any ambient or bundled DOS path.

## Consequence

The dispatcher meets the present isolation invariant: native PE targets use
ordinary `CreateProcessW`; DOS/NE targets require an explicit future BYOB
engine. R5 later established that the original historical-chain path ends at
the unrecovered `InitNtCpuInfo` generated ABI boundary. The next
runtime-bearing work is the separately named owned user-mode monitor M0; it
must not route DOS targets through the retired instrumentation or claim an
`nvtdm.exe` exists.
