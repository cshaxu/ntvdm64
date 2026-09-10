# Historical Adapter ABI Admission

Status: design gate only, 2026-08-06. No executable consumes this interface.

## Purpose

The modern research CLI and any modern-host shim use MinGW-w64 GCC. Selected
OpenNT historical modules use an i686 clang-cl/MSVC CRT island. A future
research executable may combine them only through a deliberately narrow,
versioned C ABI. This document is an admission rule for that ABI; it does not
define a substitute BOP dispatcher, DEM service surface, PC/AT device model,
or DOS runtime.

The only allowed historical execution topology remains:

```text
CCPU -> BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

An adapter may provide a host capability below an original historical caller.
It must not decode a BOP, choose a BIOS table entry, fetch a DEM service byte,
or own DOS state.

## Process and Architecture Boundary

The currently selected OpenNT CCPU profile is i686. A 64-bit process cannot
load an i686 DLL, irrespective of C ABI compatibility. Thus an executable
which directly combines the historical island and the modern shim must itself
be an i686 process. A 64-bit companion, if one is ever needed, is a separate
process and requires an explicit IPC protocol; it is not covered by this
in-process ABI.

The build systems remain separate:

```text
MinGW-w64 GCC (i686)                clang-cl/MSVC (i686)
modern CLI + modern-host shim  <--> fixed historical artifact
```

Neither CMake project becomes a subdirectory of the other. A final linker is
permitted to see only an import/static library with a recorded export set, not
the opposite island's private include directories, compiler flags, or CRT
headers.

## Mandatory Wire Rules

Every admitted declaration shall satisfy all of these rules:

1. The public header is valid C11 and wraps declarations with `extern "C"`
   only when included by C++.
2. Every integer uses `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, or the
   corresponding signed fixed-width type. `long`, `size_t`, `wchar_t`, enums,
   bit-fields, and compiler-specific `BOOL`/`HANDLE` layouts are forbidden in
   the wire layout.
3. A single explicitly named calling-convention macro is used on every
   function. For the current i686 ABI it must resolve to `__cdecl` in both
   compiler islands. The macro's expansion and decorated export name are
   recorded with the artifact.
4. The ABI begins with a `uint32_t abi_version` and a `uint32_t struct_size`.
   New fields append only; a caller rejects an older or shorter contract before
   any session is created.
5. All buffers are caller-owned. A function receives pointer plus `uint32_t`
   length/capacity and reports produced length separately. It never returns an
   allocated string, array, opaque C++ object, or CRT-owned memory.
6. No C++ exceptions, `setjmp` frames, CRT `FILE*`, `errno` storage, thread
   local data, or allocation/free pair may cross the boundary. Each side frees
   only memory allocated by its own CRT.
7. Wire structures use an explicitly documented packing/alignment rule and
   compile-time `sizeof`/`offsetof` checks in **both** islands. Pointers may
   appear only as transient caller-owned input/output buffers, never as a
   serialized or retained wire value.
8. Errors are fixed-width domain/status pairs. Raw Win32, NTSTATUS, CRT, and
   DOS error values are not silently treated as interchangeable.

## Required Admission Evidence Per Function

A proposed export is rejected unless its record identifies:

- the exact historical source caller and unresolved symbol or call boundary;
- the corresponding ledger row in
  `MODERN-HOST-SHIM-DEPENDENCY-LEDGER.md`, when it is a host capability;
- the owner of every guest register, guest-memory, DOS-state, and device-state
  mutation; and
- an i686 GCC-to-clang-cl compile/link probe proving calling convention,
  `sizeof`, `offsetof`, and buffer ownership on both sides.

The first run-level admission additionally needs a bounded trace through the
original caller and callee. A wrapper call that merely returns success is not
evidence.

## Current State

There are no admitted general-purpose ABI exports. The staging-only
`ccpu_register_abi_bridge.c` belongs to the clang-cl historical object graph;
it maps named historical DEM accessor imports to the matching CCPU `c_*`
exports and owns no register storage. It is not exported to MinGW and is not a
versioned public interface.

The CCPU generated-SAS input is not yet admitted, so no historical DLL or
adapter library may be emitted. The successful 23-object host-stack gate and
its 267-symbol external dependency inventory are classification evidence only.
They provide the source facts required to admit future individual ABI entries;
they do not make a broad host API acceptable.

## Explicitly Prohibited Shortcuts

- Linking an x64 modern runner against an i686 historical DLL.
- Passing `CONTEXT`, `TEB`, `VDM`, `SasVector`, `PDB`, `SFT`, `DTA`, or C++
  objects across the ABI.
- Exporting generic instruction-pattern trapping, arbitrary register writes,
  arbitrary guest-memory pointers, or a BOP/SVC callback registration API.
- Using the adapter to bypass original `BIOS[]`, `MS_bop_0`, `DemDispatch`, or
  DEM logic.
- Turning an admitted host capability into a host-global change, physical-drive
  access, host-clock change, injection, registry setup, or V86/HAXM path.
