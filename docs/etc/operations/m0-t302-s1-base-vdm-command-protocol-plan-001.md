# M0 T302 S1 — Base VDM command-protocol boundary and recovery plan

## Scope

Recover the first finite shared host protocol using the already mirrored
original OpenNT Base VDM client/server slice. The immediate source set is:

- `src/opennt-host/base/win32/client/vdm.c` and its reached declarations;
- `src/opennt-host/base/win32/server/srvvdm.c`, `srvvdm.h`, `srvinit.c` and
  their reached request records;
- `src/opennt-host/base/win32/inc/base.h`, `basemsg.h`, `basevdm.h` and
  `basedll.h`;
- the existing project seams `src/app/command_source.*` and
  `src/adapter-mvdm-host-out/monitor/source/vdm_command.c`.

## S1 procedure

1. Establish byte identity and original call/declaration locations for the
   reached Base VDM slice; distinguish client transport, server record state,
   copied capture, sizing/retry, wait/wake, completion and product-shell-only
   operations.
2. For each direct dependency, complete the four-rung disposition: direct
   original composition; same-shaped adapter binding; exceptional external
   intrusion; or the smallest new owned behavior. Public modern Win32/CRT is
   a leaf only when it retains the reached original contract.
3. Audit the current app command source and monitor facade as temporary seams.
   Assign each function a retain, migrate, replace or delete result. The app
   may become a bounded producer backend; it cannot remain a second Base VDM
   protocol owner.
4. Select the smallest `adapter-mvdm-host-out` subfamily that can preserve the
   original caller-facing function, structure, ordering and failure shape. A
   required new subfamily must have a named ABI and failure ledger before code
   is added.
5. Record CSRSS/CSR transport, kernel VDM, cross-process broker and WOW
   notification paths as explicit stopping boundaries. Do not crop original
   Base VDM bodies merely to avoid them.

## Expected implementation sequence after S1

The next S may compose the retained original VDM request/copy/size/retry
algorithm through the selected adapter. It must use copied fixed-width data,
session-owned state and public Win32 synchronization where necessary; it must
not retain caller pointers, native handles or host pointers across waits.

## Verification

- exact source path/hash and mirror-difference review;
- one row per reached original function and external dependency;
- an ABI/failure order review against `VDMINFO` and the Base request records;
- focused one-session positive, capacity/retry, no-command and teardown
  negative tests only after the implementation S is admitted;
- no `src.old` input, documentation governance and `git diff --check`.
