# Proposal — External system launch loader

## Status and objective

This is an unadmitted queue candidate.  It allocates no numeric task, changes
no active packet, and does not modify the `ntvdm64` runtime in advance.

Create a separate, independently built and installed system-integration
project that allows ordinary Windows launch origins to start DOS and Win16
programs through an existing `ntvdm64` runtime.  Its supported entry origins
include Explorer double-click, CMD, and covered Win32/Win64 creator processes.
The project may use an explicitly installed invasive Windows launch-hook
mechanism where required; its installation, compatibility and removal are not
part of the core runtime.

## Deliberate project boundary

```text
external system loader (32/64)          ntvdm64 runtime
-------------------------------          -------------------------------
system launch interception        ->     system32\\run16.exe
minimal local image screening     ->     NTSRV / NTVDM / NTVWM / NTCON
runtime-root configuration        ->     existing nthook32/64 descendant path
install, update and uninstall
```

The external loader must not load `ntvdm.exe`, `ntsrv.exe`, `ntcon.exe`,
`ntvwm.exe`, `ntmon.exe`, `WOW32.dll`, `VDMREDIR.dll`, or the existing
`nthook32/64.dll` into arbitrary host processes.  It launches `run16.exe` as
an ordinary process.  Once that public boundary is crossed, existing runtime
ownership and lifecycle rules apply unchanged.

`nthook32/64` remains an authenticated, NTVWM-installed in-runtime hook for
native descendants.  It is not the external global hook and must not gain a
global-injection mode merely to satisfy this candidate.

## Required launch contract

- The loader owns both 32-bit and 64-bit host integration and only the
  system-wide install/uninstall state required for it.
- It locally recognizes only safe coarse categories: DOS COM/PIF, DOS MZ that
  is not PE, and Win16 NE.  It forwards those requests to the configured
  `run16.exe` while retaining the original application request, command tail,
  current directory, environment, standard-handle intent and admissible
  creation semantics.
- Native PE32/PE32+ requests, unknown formats, scripts and unsupported launch
  APIs are passed through unchanged.  The core `run16` classifier remains the
  authoritative legacy classifier after forwarding; the loader must not
  duplicate PIF, PATH, DOS/Win16 or worker-routing policy.
- A legacy redirection returns a `run16.exe` process handle to a programmatic
  creator.  Wait/exit-code proxy behavior is a required compatibility contract;
  image-name/PID/module-list identity cannot be claimed transparent because the
  legacy target is not a native process on x64 Windows.
- The runtime root and compatible `run16` version are explicit installer
  configuration, not an ambient `PATH` lookup.  Missing, stale or incompatible
  runtime configuration fails clearly without changing a native launch.
- No NTSRV capability, worker handle, frontend token, pipe endpoint or internal
  DLL ABI crosses from the loader into an arbitrary host process.

## Proposed sequence

| S | Deliverable and stop condition |
| --- | --- |
| S1 | Audit the selected Windows global launch extension on supported host versions, including Secure Boot, 32/64 coverage, update failure modes, removal and exact process-creation interception point. Stop if it requires patching core OS files or cannot fail closed for ordinary native starts. |
| S2 | Define and test the external runtime-root/version contract plus a standalone coarse image screener. Prove DOS/Win16 selection and PE/unknown passthrough without invoking any project worker. |
| S3 | Implement the smallest 32/64 loader and installer/uninstaller in the external project. Preserve original request data and recursion protection; route only qualifying legacy launches to `run16`. |
| S4 | Verify Explorer, CMD and representative programmatic creation; test handle/wait/exit proxy behavior, missing runtime, bad configuration, recursion, native passthrough, uninstall and Windows-update recovery. |

## Acceptance

- With the optional loader installed and correctly configured, Explorer and CMD
  can launch representative DOS and Win16 programs without spelling `run16`.
- Covered external process creators route qualifying legacy targets through
  `run16`; their native children remain native and unmodified.
- The core runtime works unchanged when the loader is absent or removed.
- Install/uninstall affects only the external loader's documented system
  integration state; it does not patch OS files, modify guest media, make
  `ntvdm64` components globally injectable, or add system-wide lifecycle/task
  observation to NTSRV.
- The external project documents host-version/security limitations and has a
  deterministic disable/recovery path before any release is represented as
  automatic system-wide support.
