# M0 T373 S3 P1 — Non-fast callback source revalidation

## Question

Does the current source still contain the already-proved, original non-fast
`CallBack16`/CPU40 transaction, and which current task-state carrier actually
feeds its `CURRENTPTD()` prerequisite?

## Inputs

* Original mirror `src/mvdm-host/wow32/wcall16.c`.
* Original CPU40 `host_simulate` carrier
  `src/mvdm-host/softpc.new/host/src/nt_cpu.c`.
* Existing bounded lease adapter
  `src/adapter-mvdm-host-out/wow/wow_callback_frame_lease.{c,h}`.
* Existing private-TLS OpenNT carrier
  `src/adapter-mvdm-host-out/win32/include/nt.h` and
  `opennt_support_rtl.c`.
* Prior source/link/return evidence P8 and P12 from T335 S3.

## Observations

* The selected original `CallBack16` body remains in `wcall16.c`. Its only
  callback-specific mirror divergence is `MVDM-HOST-DIV-178`: three former
  raw `VDMFRAME`/`CBVDMFRAME` alias intervals acquire/release bounded
  adapter-softpc guest-memory leases. The original frame arithmetic, task
  identity checks, callback-frame writes, `SETVDMSTACK`, `host_simulate`, IP
  restoration, AX:DX extraction and VDM stack restoration remain in the
  original body and in the original order.
* `CURRENTPTD()` still uses the original spelling
  `NtCurrentTeb()->WOW32Reserved`. `ADAPTER-WIN32-041` provides that field in
  a thread-private OpenNT-shaped TLS carrier. This is the actual selected
  source-compatible task-state route.
* The separate `adapter-mvdm-host-out/monitor/mvdm_wow_task_frame` experiment
  is not compiled into any current formal product or callback graph and is not
  called by `wcall16.c`. It cannot be represented as the current callback
  provider or used to claim that an original WOW task is bound.
* P12's original x86 CCPU40 focused proof was committed after all currently
  selected callback source/adapter files. No later commit changes the selected
  `wcall16.c`, lease adapter, SoftPC guest-memory adapter, `nt_cpu.c`,
  `nt_aorc.c` or the private-TLS carrier. It remains valid source/link evidence
  for the narrow non-fast recursive return transaction.
* The proof deliberately stops before the real `WOW16_From_CallBack16` guest
  trampoline and before full WOW task creation. Those conditions still require
  the original WOW32 product shell recorded by S2 P2.

## Disposition

The current S3 scope is the original non-fast mechanical callback transaction,
not a second task projection or an early WOW task lifecycle. It reuses the
existing source-shaped `WOW32Reserved` TLS compatibility binding and bounded
lease only. No production callback source was changed in this revalidation.

## Verification

* Exact source and caller audit of `CallBack16`, the lease boundary and
  `CURRENTPTD()` carrier.
* Commit-range audit from the prior P12 closure to current HEAD finds no change
  in the selected callback surface.
* P12 retains the formal x86 CCPU40 link and successful bounded-return result;
  this P1 does not extend that result to actual Win16 trampoline execution.

## Follow-up

S3 next reconciles the actual callback return trampoline and first original
task-entry prerequisite against S2's unavailable `W32Init` boundary. It must
not activate the unselected task-frame experiment, FastWOW or a synthetic
Win16 callback path.
