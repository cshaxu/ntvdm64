# M0 T373 — WOW16 bootstrap, callback and Win16 NE lifecycle closure

## Delivered scope

T373 recovered the whole original source topology from the dynamic `MS_bop_1`
ingress through WOW32 command/task dispatch, non-fast `CallBack16`, guest
WOWEXEC/Kernel NE ownership and normal return.  It did not create a static
route, substitute WOW32 DLL, synthetic task/NE loader or guest-media change.

The selected original ingress requires `WOW32` to load and initialize its
private USER/shared-session product shell before `W32Dispatch` can reach
`WK32WOWInitTask`.  The present formal CPU40/x86 graph selects no WOW32
provider, and its fixed runtime package stages no WOW16 media.  Therefore the
source-defined dynamic-load failure direction is the exact current product
disposition.

## Evidence

* S1: `docs/etc/evidence/m0-t373-s1-wow16-bootstrap-topology-ledger-001.md`.
* S2: `docs/etc/evidence/m0-t373-s2-p1-wow-command-binding-dependency-closure-001.md`
  and `m0-t373-s2-p2-wow32-dynamic-ingress-boundary-001.md`.
* S3: `docs/etc/evidence/m0-t373-s3-p1-nonfast-callback-source-revalidation-001.md`.
* S4: `docs/etc/evidence/m0-t373-s4-win16-ne-lifecycle-boundary-001.md`.
* S5: `docs/etc/evidence/m0-t373-s5-formal-selection-stage-reconciliation-001.md`.

## Verified limits

* No fixed Win16 observation was eligible; none was run.
* No claim is made that WOWEXEC, a Win16 NE module, a callback trampoline or
  general WOW services execute.
* Fast WOW, CurrentMonitorTeb, CSRSS/kernel-VDM transport, multi-session
  WOW, private USER/GDI product behavior and a substitute WOW32 DLL remain
  outside this closed packet.

## Next work

The next dependency-ordered candidate is the WOW16 service, UI and host
integration matrix in `docs/QUEUE.md`.  Any future activation must admit one
complete original WOW32 cohort and its immutable media together; it may not
turn this exact-unavailable disposition into a static `W32Dispatch` shortcut.
