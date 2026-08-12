# Default Build Instrumentation Isolation

Status: verified build-graph boundary, 2026-08-06.

## Decision

The normal executable is `ntdos64-run.exe`. It is only a non-invasive CLI
dispatcher: PE32/PE32+ input is handed to `CreateProcessW`; DOS/NE input
requires an explicit user-supplied engine. It does not contain or select a DOS
runtime.

The following self-authored research paths are **not** a runtime and are
exposed only as a source-only reference target with
`-DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=ON`:

* `ntvdm-startup-reference`: the former `nvtdm` lifecycle, private bootstrap
  rewrite, IVT/BDA, single BOP/DEM, DOS namespace, and staged-image probes.
* The self-authored BOP observer and its tests.
* The self-authored WHPX DOS
  execution experiments.

The reference target produces no executable or library, even when enabled.
They may be read, but cannot execute through this build graph. A future trace
tool may record, assert, or snapshot a bounded condition only. It must not
modify BOP/SVC meaning, DOS return data, device response, or a bootstrap-success
result, and it cannot be supplied as the normal `ntdos64-run --engine` route.

## Formal Runtime Admission Gate

No target currently claims to be the historical NTDOS runtime. A future
runtime target may be added only after a link evidence record proves this
original chain, without self-authored replacement handlers:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

The required evidence must identify the source/object provider of each arrow,
the exact modern host shims (if any), and a bounded execution trace through
the original dispatcher. A successful self-authored BOP, SVC, IVT/BDA, or DOS
probe is explicitly insufficient.

## Default-Build Evidence

Fresh configuration used:

```text
cmake -S . -B artifacts/build/legacy/referenced/build-isolation -G "NMake Makefiles"
  -DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=OFF
cmake --build artifacts/build/legacy/referenced/build-isolation
ctest --test-dir artifacts/build/legacy/referenced/build-isolation --output-on-failure
```

Result: build succeeded and all four registered default tests passed:
`whpx-real-mode-hlt`, `whpx-real-mode-io`, `whpx-real-mode-int`, and
`opennt-reloc`.

The generated `CMakeFiles/Makefile2` lists no default target named
`nvtdm`, `ntvdm-startup-reference`, `owned-dos-engine`, `whpx-com-run`,
`whpx-bop-observe`, `bop-rewrite-test`, `transition-registry-test`, or
`bootstrap-profile-test`. The generated `CTestTestfile.cmake` lists none of
their tests.

`CMakeFiles/ntdos64-run.dir/objects1.rsp` contains only:

```text
CMakeFiles/ntdos64-run.dir/src/runner/ntdos64_run.c.obj
```

Its generated linker rule references ordinary Win32 libraries, including
`shell32.lib`, but none of the self-authored instrumentation objects. This is
build-graph evidence only; it does not claim that an NTDOS runtime now exists.

The source-only opt-in was also verified separately:

```text
cmake -S . -B artifacts/build/legacy/referenced/build-reference -G "NMake Makefiles"
  -DNTDOS64_ENABLE_TRACE_INSTRUMENTATION=ON
cmake --build artifacts/build/legacy/referenced/build-reference --target ntvdm-startup-reference
```

That command completed without compiling any project source or producing any
project executable, DLL, or library. The only matching executable under that
build directory was CMake's own compiler-identification helper. Its generated
`CTestTestfile.cmake` still contained only the same four normal tests. Thus
the opt-in reference target exposes source provenance without creating a
backdoor executable path for the former self-authored runtime.

## Entry And Stale-Artifact Audit

The current `src/runner/ntdos64_run.c` entry point has no reference to `nvtdm`, the
former bootstrap profile, a BOP observer, or a DOS-service module. It hands
PE32/PE32+ images to `CreateProcessW`; for DOS MZ/COM and NE images it returns
an explicit-engine error unless the caller supplied `--engine`.

The archived `artifacts/build/legacy/referenced/build-root-pre-isolation/`
tree contains generated `nvtdm.exe`, its CTest registration, and object files
from a pre-isolation configuration. Those are stale generated artifacts, not
inputs to the current CMake graph, and must not be used as runtime or
acceptance evidence. The authoritative verification
directories are the fresh `artifacts/build/legacy/referenced/build-isolation/`
and `artifacts/build/legacy/referenced/build-reference/` trees
described above. The normal runtime must continue to be assessed only from a
fresh configuration of the current source tree.

## Revalidation After Historical-Input Expansion

On 2026-08-06, after adding the default-disabled cross-version EMS recovery
candidate under `src/opennt-45`, the existing MinGW verification build was
rebuilt with:

```text
cmake --build artifacts/build/current/default-mingw-verify --target ntdos64-run --parallel 4
ctest --test-dir artifacts/build/current/default-mingw-verify --output-on-failure
```

`ntdos64-run` built successfully. All four registered tests passed:
`whpx-real-mode-hlt`, `whpx-real-mode-io`, `whpx-real-mode-int`, and
`opennt-reloc`. A source/CMake search finds the `opennt-45` path only in
the standalone `src/opennt/CMakeLists.txt` compiler island; the root runner
graph does not reference it. This is isolation evidence only and does not
promote the EMS candidate or any historical object to a runtime component.
