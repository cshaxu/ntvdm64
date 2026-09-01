# M0 T335 — Original WOW32/Win16 owner-package recovery

## Intent

Recover the original OpenNT WOW32 host package and its Win16 guest boundary as
one source-owned package.  This task begins from the byte-preserved
`mvdm-host/{wow32,fax}` mirror and the independently load-only
`mvdm-guest/win16` media.  It does not turn either package into a new Win16
API implementation.

The selected machine is the original SoftPC CCPU40 path.  All source-facing
CCPU/SAS/monitor calls retain their original names and ABI shape through
`adapter-mvdm-host-out/softpc` and `adapter-mvdm-host-out/monitor`; no Bochs
route is a production input.  Guest addresses use the session-owned
guest-memory mapping instance, while persisted host resource identities use
the session-owned host-resource instance.

## Fixed boundary

- `mvdm-host/wow32` owns original WOW32 dispatch, thunk, frame, lifecycle,
  shell and optional GUI provider flow.  `mvdm-host/fax` owns its original
  FAX adjuncts.
- `mvdm-guest/win16` is load-only guest media: it is never a host static-link
  input.
- `adapter-mvdm-host-out/wow` is reserved for same-shaped WOW-specific
  binding only.  `adapter-mvdm-host-out/win32`, `softpc`, `monitor` and
  `basesrv` retain their respective historical interface families; no generic
  compatibility component is introduced.
- Public Win32 can provide a source-shaped host capability only where its
  original caller layout, lifetime, failure and ordering have been proven.
  Private CSRSS/BaseSrv transport, kernel VDM, private USER/GDI, VDD and
  cross-process WOW product shells are not silently recreated.

## Subtask sequence

1. **S1 — source/ABI and profile rebaseline.** Reconcile the complete current
   WOW32/FAX mirror, selected `sources` entries, the original guest media
   boundary, all present adapter declarations, each mapping-instance crossing,
   and every inherited disposition.  Refresh the old T291 evidence under the
   current CCPU40-only architecture; select one smallest coherent original
   owner cohort only after this audit.
2. **S2 — source-shaped control/frame cohort.** Recover the selected original
   `W32DllInitialize`/task-frame/callback control cohort using existing
   same-shaped SoftPC, monitor, session and Base VDM forms.  Preserve source
   ordering and original failure behavior.  This step does not enable GUI,
   WOWEXEC, Win16 API thunks or a guest load merely because source compiles.
3. **S3 — declared non-GUI vertical profile.** Compose the smallest complete
   original non-GUI profile: source initialization, bounded command/control
   handoff, mapped guest frame transaction, CCPU40 resume/stop postcondition,
   and the exact completion/unavailable route.  GUI/USER/GDI, printing,
   multimedia, OLE, network and FAX must each retain an explicit original or
   successor-owner disposition.
4. **S4 — package reconciliation.** Build selected original source and
   adapters on fresh x86 and x64 formal graphs; run focused source-contract
   checks and at most one fixed-container observation.  Reconcile every
   selected row, divergence and owner transfer before closing T335.

## Completion standard

T335 may claim only a source-backed, declared non-GUI WOW32/Win16 host/guest
profile.  Every selected original file and interface must have a direct,
binding-only, adapter-backed, exact-unavailable or named successor-owner
disposition.  Both formal architectures must build the selected closure.  It
must not claim generic Win16, GUI, WOWEXEC, CSRSS/BaseSrv, kernel VDM,
cross-process broker or arbitrary guest-application execution until those
complete owner contracts are separately proven.
