# T95 S6 Narrow Companion-Link Design

## Decision

The first adapter-bearing executable, if separately admitted, is a distinct
MSVC/x86 observation target named
`ntdos64-native-adapter-observation.exe`. It is not a replacement for the r4
`ntdos64-native-container.exe`, which remains adapter-free and is the only
default native target.

## Exact ownership and direction

```text
MinGW runner/CLI
  -> immutable child environment: profile path + BYOB root
  -> MSVC/x86 companion process
     -> narrow adapter endpoint and startup-session objects
     -> generic Bochs plan consumer
     -> original Bochs PC reset-floor machine
```

The process boundary is the CLI-to-child environment already established by
the runner. No object, allocator, pointer, C++ type or handle crosses the
MinGW/MSVC boundary. Within the companion process, the narrow C adapter owns
the verified BYOB buffers and provides the existing pointer-free plan plus its
same-island payload pointer; Bochs owns every machine-memory copy and CPU
state update. Bochs never sees the profile/root strings.

## Build inputs

Use a new, immutable build root derived from the exact r4 native tuple. Copy
the pinned Bochs source/configuration/Makefile family and the following narrow
sources and headers into build-local `adapter/` and `cli/` directories:

1. `bx_ntvdm_startup_session_environment.c`
2. `bx_ntvdm_startup_session.c`
3. `bx_ntvdm_cpu_state_abi.c`
4. `bx_ntvdm_guest_range.c`
5. `bx_ntvdm_guest_write_abi.c`
6. `bx_ntvdm_startup_plan_abi.c`
7. `byob_profile.c`
8. `byob_identity.c`
9. `byob_image.c`

Only `main.o` is recompiled from Bochs, with
`/DBX_NTVDM_ENABLE_EXECUTION_PLAN=1` and the build-local adapter include path.
All other Bochs objects/libraries and the original r4 link vector remain
unchanged. The 9 C objects compile with the same x86 `/MT /W3 /DWIN32` CRT
model, using only copied `adapter/` and `cli/` include directories.

The new target repeats r4's explicit `link` vector and appends exactly the nine
adapter/CLI objects plus `kernel32.lib` and `bcrypt.lib`. `uuid.lib` remains a
recorded default-library directive until the companion link proves whether it
is retained. No Bochs Makefile, configure result or default target is changed;
the companion shim owns the replacement target and explicit `main.o` recipe.

## Link and runtime gates

The first admitted link may prove only MSVC/x86 link closure. It must use a
fresh root, preserve response/shim/command/log/map/import/undefined-symbol
evidence, and stop on the first error. A successful link does not prove that
the adapter endpoint is called.

The first runtime gate is narrower still: invoke the linked companion with no
adapter environment. It must take the original no-plan path and produce the
same bounded PC reset-floor stop class as r4. A separate admission is required
before any environment-driven payload, including a synthetic HLT identity; a
source-built NTIO handoff is later again.

## Rejected alternatives

- Linking `bx_ntvdm_adapter_runtime.c` or its 38-source closure: would import
  host/service and DOS-facing semantic ownership before it is reached.
- Adding adapter objects or an opt-in macro to r4/default Makefiles: would make
  a default Bochs product depend on composition code.
- Linking MinGW runner objects into Bochs: violates CRT/toolchain-island
  ownership.
- Passing guest payload bytes through CLI argv/environment: makes the runner a
  guest loader and leaks guest input into Bochs.
