# M0 T307 S1 — Base VDM source-shaped broker recovery

## Objective

Recover the directly reached, one-session DOS Base VDM command-contract
surface from the original `vdm.c` client and `srvvdm.c` server ordering. The
runtime keeps the original public spellings and `VDMINFO` layout, while the
smallest `adapter-mvdm-host-out/basesrv` facade replaces only the unavailable
CSR capture/CSRSS console-record transport.

## Source-first route

- Client source: `opennt-host/base/win32/client/vdm.c`, specifically
  `GetNextVDMCommand`, `ExitVDM`, `SetVDMCurrentDirectories` and
  `GetVDMCurrentDirectories`.
- Server source: `opennt-host/base/win32/server/srvvdm.c`, specifically the
  DOS branch of `BaseSrvGetNextVDMCommand`, `BaseSrvIsFirstVDM`,
  `BaseSrvSetVDMCurDirs`, `BaseSrvGetVDMCurDirs` and the DOS exit disposition.
- The CSR capture-buffer and BaseSrv console/global-record product shell is a
  T306-proved non-invasive hard boundary. It is not imported or recreated.
- Existing adapter code is retained only where it preserves the reached
  source's copy/capacity/retry, first-VDM, reentry, directory and terminal
  ordering. New mechanics belong in the named `basesrv` adapter, never app or
  an OpenNT mirror file.

## Delivery sequence

1. Compare the current local seam against the reached original forms and
   identify each missing one-session DOS behavior and its failure result.
2. Recover the first-VDM query, copied current-directory set/get operations,
   and terminal DOS `ExitVDM` disposition through the session-owned local
   record. Preserve `GetNextVDMCommand`'s existing copy/capacity/reentry
   behavior rather than adding a parallel protocol.
3. Add focused positive/negative fixtures for the whole admitted API family on
   both MSVC architectures and link them through the formal Ninja graph.
4. Record every retained CSRSS/WOW/PIF/cross-process/child branch as a later
   owner or hard boundary; do not simulate it as success.

## Exclusions

No BaseClient/BaseSrv product-shell import, CSR/CSRSS recreation, generic
queue/broker, WOW routing, PIF, child execution, BOP service, guest trace,
kernel VDM call or Bochs/SoftPC change is admitted.

## Exit

The full directly reached one-session DOS Base VDM family has a source-shaped
implementation or explicit boundary disposition; all implementation is in the
named adapter facade, the original mirror body remains minimally changed, and
the focused x86/x64 regression is recorded.
