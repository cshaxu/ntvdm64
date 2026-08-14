# M0 T198 S131: COMMAND 54:04 Reuse and Admission Map

## Original owner and contract

Pinned `base/mvdm/inc/cmdsvc.h` names service `04h` as `SVC_CMDGETCURDIR`.
`dos/command/cmddisp.c` sends it to `cmdGetCurrentDir` in `cmdmisc.c`.
The original receives a zero-based drive in AL and a buffer in DS:SI. It uses
`demGetPhysicalDriveType`, host drive probing, and per-drive `=X:` environment
state. A valid first query writes the drive root, persists it, and clears CF.
A missing drive or oversize existing state returns CF set with AX zero.

This original object is not CLI-linkable: drive probing, environment mutation,
and VDM pointer access are historical host-composition mechanisms.

## Existing candidate and classification

`src/bx-vdm/bx_ntvdm_cmd_current_dir_service.c` is a bounded source-derived
candidate. It accepts only real-mode `C4 C4 54 04`, takes AL as the drive,
uses an immutable available-drive bitmask, and prepares one checked RAM write
of a root string at DS:SI for an admitted drive. An unadmitted drive returns
the documented CF/AX-zero result without a write. It deliberately neither
persists `=X:` state nor scans a host drive, opens a path, or models arbitrary
DOS directories.

The candidate remains only in detached `bx_ntvdm_adapter_runtime.c` and
`bx_ntvdm_startup_transaction_bridge.c`; it is absent from the T198 manifest
and active composition. Global classification already identifies service 4 as
deferred COMMAND `LAUNCH`.

## Disposition

**Reuse with bounded adaptation.** The next slice may add it to the composed
plane only after common ingress, provider selection and COMMAND `LAUNCH`
classification establish exact service 4 identity. It must use only the
composition's immutable drive snapshot and generic mantle multi-write
mechanics. It must not import legacy gates, host-drive scanning, environment
persistence, path search, DOS filesystem behavior or direct `CmdDispatch`.

A focused adapter-only test must cover admitted root write/CF-clear,
unadmitted direct CF/AX-zero, and malformed/non-real-mode decline before any
native observation is admitted.
