# M0 T259 S2 P1 — RTL Contract And Status Recovery

## Scope

This first S2 slice consolidates the active COMMAND `HOST-07` declaration
surface and restores one previously collapsed original semantic operation:
`RtlNtStatusToDosError`.

## Source and boundary

| Item | Record |
| --- | --- |
| Original callers | `base/mvdm/dos/command/cmdexec.c` (`cmdCheckBinary` and `cmdCreateProcess`) and `base/mvdm/dos/dem/demsrch.c` call `RtlNtStatusToDosError`. |
| Historical interface | `ULONG RtlNtStatusToDosError(NTSTATUS)`, counted-string layouts and allocation/free pairing used by the imported COMMAND files. |
| Active mirror | `src/bx-vdm/bop/opennt/command/{cmdexec,cmdenv,cmdconf,cmdmisc}.c`. |
| Modern recovery | `src/bx-vdm/bop/shim/opennt_rtl_compat.h` owns the canonical same-shaped declarations and host-private layout assertions. `command_binary_shim.c` resolves the current `ntdll.dll` export once, with a deterministic fallback for statuses emitted by the local facade. |
| Intrusion result | No `src/bx-core`, `src/bx-mantle`, or `src/bx-vdm/bop/opennt/**` file changed. The entire change is a `bx-vdm` shim/header boundary plus a project fixture and its formal link membership. |

## Semantic result

The previous shim returned `ERROR_INVALID_DATA` for every `NTSTATUS`, which
lost original COMMAND/DEM error differentiation. The recovered facade now
uses the same-named current OS conversion when present and retains a bounded
fallback for `STATUS_SUCCESS`, invalid parameter, no memory, object/path not
found, access denied and invalid handle. It preserves the imported call shape
and lets the original branch receive its corresponding Win32/DOS error.

The counted-string records are explicitly host-private native-pointer records:
their `USHORT` length fields and first-member offsets are asserted, while their
pointer fields never enter guest state. This supports both x86 and x64 without
truncating host pointers.

## Formal evidence

1. Generated a fresh disposable MSVC x64 `/MT`, CPU5/P-MMX graph at
   `build/M0-T259-S2/formal-r3` from the reviewed manifest.
2. Corrected the pre-existing `t231-s6` target link closure to declare all
   transitive static libraries it requires: `bx-vdm`, `bx-mantle`, `bx-core`,
   and `opennt-host`.
3. External Ninja built the full 286-node prerequisite closure and linked
   `bin/t231-s6-command-check-binary-direct-fixture.exe`.
4. The fixture checks `STATUS_INVALID_PARAMETER` converts to
   `ERROR_INVALID_PARAMETER`, then passes the source-mirror `cmdCheckBinary`
   handoff, guest writeback and DOS-bypass regression:

   ```text
   T231 S6 direct OpenNT cmdCheckBinary handoff, session writeback, and DOS bypass verified
   ```

## Remaining S2 work

P1 establishes the canonical declaration/status seam; it does not close S2.
The next slices must reconcile allocation ownership and conversion behavior
across the active COMMAND and DEM consumers, then run their focused
environment/conversion regressions. No imported OpenNT or Bochs body is to be
edited merely to simplify that work.
