# T95 S7 Post-Drive BOP Source Audit 002

## Question

What do the first two unserved BOP identities in runtime trace 003 mean, and
can their already authored adapter contracts be admitted without extending the
Bochs machine or turning the adapter into a DOS filesystem?

## Inputs

- Runtime trace: `artifacts/analysis/t95-s7-runtime-trace-003-20260811-001`.
- OpenNT source: `src/opennt/base/mvdm/inc/dossvc.h`,
  `src/opennt/base/mvdm/dos/dem/demgset.c`,
  `src/opennt/base/mvdm/inc/cmdsvc.h`,
  `src/opennt/base/mvdm/dos/command/cmdmisc.c`, and
  `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm`.
- Existing adapter units:
  `src/bx-ntvdm-adapter/bx_ntvdm_dem_boot_drive_service.c` and
  `src/bx-ntvdm-adapter/bx_ntvdm_cmd_current_dir_service.c`.

## Procedure

1. Decode the trace selectors against the OpenNT definition headers and their
   dispatcher/caller sites.
2. Compare entry, result register, carry-flag and guest-memory effects with
   the two existing adapter unit contracts.
3. Register only those units in `bx_ntvdm_adapter_runtime` and run the
   focused runtime regression.

## Observations

- `dossvc.h` defines `SVC_DEMGETBOOTDRIVE` as `0x0d`; free-space is `0x0e`.
  `sysinit1.asm` consumes `50:0d` into `default_drive` and `sysi_boot_drive`.
  `demGetBootDrive` documents AL as a one-based drive and initializes its
  registry-unavailable fallback to C (`AL=3`).
- `cmdsvc.h` defines `SVC_CMDGETCURDIR` as `0x04`. `cmdGetCurrentDir` receives
  DS:SI and AL, writes `X:\\` on a first valid-drive query, clears CF, and on
  an invalid drive sets CF with AX=0. DOS kernel `macro.asm` uses it while
  synchronizing a CDS marked `CURDIR_tosync`.
- The adapter boot-drive unit performs only the documented AL=3 fallback and
  resumes. The current-directory unit performs one bounded four-byte generic
  multi-write for a profile-admitted letter, or returns the documented direct
  CF/AX failure with no queued write.
- `bx_ntvdm_adapter_runtime` now derives its valid-letter mask solely from the
  immutable 26-entry profile inventory already installed for the session. It
  neither calls the host drive scanner nor opens a host path on either BOP.
- The C11 target `bx-ntvdm-adapter-runtime-test` rebuilt and exited zero. It
  covers `50:0d` AL preservation/fallback, `54:04` C-root write at DS:SI, and
  invalid-drive CF/AX with no pending multi-write.

## Interpretation

These are source-proven host-composition contracts, not evidence of missing
CPU, firmware, device, DOS-kernel, or filesystem behavior. Their admission
uses existing typed CPU-result and ordinary-RAM multi-write mechanics. Bochs
continues to see neither selector nor path semantics.

## Confidence and limits

High confidence for the first contained profile's C fallback and root current
directory. This does not admit `50:0e` free-space, arbitrary per-drive current
directories, environment-variable persistence, host-drive rescans, search,
or general DOS filesystem behavior.

## Follow-up

Create a fresh refusal-first S7 r6 build root with this expanded adapter source
closure, build it once, then perform one watchdog-bounded trace. Classify the
next reached BOP from its OpenNT caller before adding any further service.
