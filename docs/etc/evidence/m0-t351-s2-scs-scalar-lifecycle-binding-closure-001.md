# M0 T351 S2 — SCS scalar-lifecycle binding closure

## Result

The selected original COMMAND/DEM/SoftPC SCS scalar cohort is closed as one
source-shaped, session-bound binding.  No new BOP provider, guest mutation,
mapping manager, durable host pointer, or substitute CPU path was added.

## Original contract and selected binding

`dos/command/cmdmisc.c::cmdSetInfo` originally obtains four flat aliases from
the current VDM address space: the `SCSINFO` record at `DS:DX`, and the
`SCS_ToSync`, `SCS_Is_Dos_Binary`, and `SCS_FDACCESS` scalar locations at the
original register-specified positions.  NT4 retained those aliases in host
globals for later COMMAND, DEM and SoftPC consumers.

The selected mirror retains the original function, dispatcher selection,
register reads, scalar ordering, and caller-visible CF/AX direction.  It
registers the corresponding 16:16 numeric locations through
`mvdm_command_guest_state_set_scs_scalars`.  The same thread-bound state owns
the session and guest-memory lease epoch.  A later consumer acquires a short
bounded lease only for its immediate byte or `WORD` operation.  Missing,
wrong-session, stale-epoch, or out-of-range accesses fail through the existing
`ERROR_INVALID_ADDRESS` direction rather than publishing a host alias.

This is the minimal required divergence from the original pointer-retention
implementation and remains registered as `MVDM-HOST-DIV-111` and
`ADAPTER-SOFTPC-037`.

## Complete selected-consumer sweep

- `cmdmisc.c` registers the locations and writes `SCS_ToSync` /
  `SCS_Is_Dos_Binary` through the same state.
- `cmdexec.c` acquires `SCSINFO` only locally under an active short lease and
  releases it on every selected exit path; it publishes no durable alias.
- `cmdpif.c` writes the original DOS-binary scalar through the same binding.
- `demmisc.c` resets the FD-access scalar through the same binding.
- `nt_fdisk.c` and `nt_rflop.c` read/update the original `SCS_FDACCESS` word,
  retaining its unsigned modular arithmetic.
- `cmddata.c` contains no remaining durable `pSCSInfo`, `pSCS_ToSync`,
  `pIsDosBinary`, or `pFDAccess` host-pointer global.

The `wow32/{wkman,wdos}.c` raw `DOSWOWDATA.lpSCS_ToSync` form is not part of
this one-session CPU40 bootstrap cohort.  It remains a future WOW owner item;
S2 deliberately does not reintroduce a global alias to make it appear solved.

## Verification

- The existing x86
  `tests/adapter-mvdm-host-out/softpc/command-scalar-state-fixture.c` rebuilt
  and passed.  It covers the three scalar locations, byte and little-endian
  `WORD` access, `WORD` wrap, wrong-session rejection, inactive-lease
  rejection, restarted-epoch rejection, and range refusal.
- A fresh formal CPU40/x86 graph at
  `build/M0-T351/S2/record-x86` completed the 425-action
  `original-softpc-process.exe` link.  The resulting product is
  `build/M0-T351/S2/record-x86/original-softpc-process.exe` (3,104,768 bytes).
  Compiler warnings remained visible; none was suppressed.
- Source comparison confirms that the selected original COMMAND dispatcher and
  CPU40/SAS execution body remain the owner.  S2 does not claim that the
  fixed runtime observation has returned from `54:05` or reached NTDOS
  `msinit.asm`; that is the purpose of later S3/S4 evidence.

## Disposition

S2 satisfies its source-binding exit criteria.  S3 may use the existing
source-shaped register-boundary fixture and a fresh formal link to freeze the
complete local contract before the single fixed-container observation.
