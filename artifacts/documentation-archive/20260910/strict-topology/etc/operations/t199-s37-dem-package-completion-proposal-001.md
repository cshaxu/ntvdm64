# M0 T199 S37 DEM Whole-Package Completion Proposal 001

## Question

What exact work remains to close the active DEM provider package without
turning individual trace hits into independent implementation work or moving
DOS, device, or Bochs semantics into `bx-vdm`?

## Scope and authority

This is the detailed implementation plan for the active `M0 T199 S37` packet
in `docs/STATUS.md`.  It does not allocate another T or S, replace the active
packet, or make a completion claim.

The selector and service authority is OpenNT
`src/opennt/base/mvdm/dos/dem/demdisp.c`: `50:00..48` are 73 callable DEM
members and `50:49` is the sentinel.  Historical handler source under
`src/opennt/base/mvdm/dos/dem/` determines calling layout, ordering, and
failure behavior.  The current owner implementation is the package session
under `src/bx-vdm/`, not direct linkage of historical `DemDispatch`.

## Completion definition

A service is closed only when it has exactly one of these dispositions and a
family regression proves it:

1. original OpenNT host provider that independently composes;
2. the same original provider with a declared contained CLI capability;
3. a minimal source-derived rehost of the recorded ABI and failure contract;
4. the individual original no-op or source-proven unavailable/failure result.

Routing a BOP to a typed result is necessary but insufficient.  A source
derived failure is a closed contract, but it is not a claim that the original
host facility is available.  No disposition may perform a BOP-time ambient
host query, create a raw disk/device backend, or leak a generic CPU
pass-through.

## Current position

| Area | Existing bounded behavior | Still required for S37 closure |
| --- | --- | --- |
| Common DEM session | All 73 callable identities enter the package session and receive a typed result; no raw `#UD` route remains. | Keep this all-family invariant while replacing/confirming every generic fallback with the selected identity's source-derived terminal contract. |
| Immutable namespace | Selected readonly open/read/close/seek flows exist; mutating pathname/file operations return a contained DOS access-denied result. | Audit every handle/path member for its exact failure and invalid-input behavior; do not add a writable ambient filesystem. |
| FCB/search | `50:0A/0C` implement bounded profile-entry first/next search; `50:2E` null-close and `50:30` date/time have dedicated contracts. | Finish malformed layout, continuation, create/open/I/O/rename disposition as one FCB provider family.  A write-capable FCB backend requires separate contained-capability admission; it is not presumed. |
| GSET/drive state | Boot drive, drive list, free-space, media-ID get, DTA, date/time, full `50:25` fake DPB, `50:41` no-identity failure, and the guarded `50:1A` no-current-directory terminal disposition have bounded behavior. `50:10` set is CF-only failure. | Audit default/current-drive state and all GSET invalid-address/drive cases as one component. `50:46` remains deliberately limited to Drive/Unit/Next, as in OpenNT. |
| DASD/IOCTL | Metadata-only IOCTL is admitted; unsupported IOCTL is AX=1/CF and absent-BDS absolute I/O is AX=21/CF. | Regress every subfunction and retain the no-device boundary.  No INT13, raw host drive, or synthetic BDS enters `bx-vdm`. |
| Error/lock | Registration, no-retry failure, and contained lock failure are present. | Audit retry lifetime, invalid registration and all relevant error propagation as one session component. |
| DOS/misc/lifecycle | Bounded DOS-image load and selected non-debug/non-WOW/no-device normal-return paths exist. | Give every V86, VDD, debug, console, process, pipe and WOW member an explicit original owner and terminal/capability disposition.  Do not manufacture a host process manager. |
| Original no-ops and FastRead compatibility | Original no-op members continue with CF clear; the separately admitted readonly FastRead compatibility route is retained. | Regress no-op versus compatibility precedence and make every exception visible in the final disposition ledger. |

## Ordered work packages inside S37

The order follows source ownership and shared ABI, not observed BOP order.

### A. Finish GSET as one immutable state component

1. Record the packed OpenNT `DPB` layout from `demdasd.h`; retain the existing
   `50:46` rule that writes only Drive/Unit/Next.  `DPB.Next` is offset 27,
   after MediaID and FirstAccess.
2. Retain `50:25 demGetDPB`'s admitted immutable-volume fake-DPB branch and
   its unadmitted-drive terminal result. Do not claim a BIOS BPB/BDS exists.
3. Retain `50:41 demGetComputerName`'s source-shaped empty/failure branch
   until a stable configured identity is separately admitted. Never expose
   the live host machine name at BOP time.
4. Retain `50:1A`'s first-byte guard and contained no-current-directory
   disposition until a reproducible guest session capability is admitted.
5. Verify `0D-10, 14-15, 19, 1A-1C, 25, 41, 46`, including unavailable drive,
   bad guest range, setter, and two-drive list cases.

### B. Finish namespace and FCB as separate providers

1. Reconcile all `demhndl.c`/`demfile.c`/`demdir.c` entries against the
   immutable namespace and source error values.
2. Reconcile every `demsrch.c`/`demfcb.c` entry, including foreign PDB,
   malformed continuation, FCB create/open/read/write and rename outcomes.
3. Keep successful profile search bounded to the declared guest namespace;
   do not infer host-directory enumeration or writable DOS storage.

### C. Finish terminal host-composition families

1. Complete `demerror.c`/`demlock.c` state and failure regression.
2. Complete `demmisc.c`/`demmsg.c` dispositions for lifecycle, V86, VDD,
   debug, console, pipe and WOW members as a group.
3. Recheck DASD/IOCTL subfunction coverage and no-device negative boundary.

### D. Package closure evidence

1. Produce one final ledger containing all 73 service identities, original
   owner, selected provider kind, ABI/memory effect, normal outcome, failure
   outcome, and regression name.
2. Run the all-family source-built x64 `/MT` regression with success,
   source-proven failure, invalid-address, and no-bypass cases for every
   provider family.
3. Only then run one bounded native trace as integration verification.  A new
   BOP seen by that trace is observation only; it cannot change the package
   plan without owner re-admission.

## Dependencies and boundaries

- CLI admission owns capture of allowed drives and any allowed immutable
  metadata.  `bx-vdm` receives copied snapshots and never retains paths,
  handles, or host API objects.
- `bx-vdm` owns BOP routing, fixed-width register results, and checked guest
  RAM transactions.  It does not own DOS kernel, filesystem, CPU, firmware,
  interrupt, PIC, disk device, VDD, or GUI semantics.
- `bx-mantle`/`bx-core` execute only typed mechanical RAM actions.  DEM work
  does not admit another Bochs device or selector-aware Bochs patch.
- A historical provider is preferred whenever it can compose independently;
  otherwise a minimal source-derived provider records the historical blocker.

## Acceptance evidence

The final S37 claim requires: the canonical dispatch audit; the per-service
ledger; source/ABI/failure records for every provider; a source-built all-DEM
family regression; a review proving no `package-deferred`/raw pass-through
route; and one bounded native integration trace.  Until all five exist, DEM
is not complete even if the 73-entry routing sweep passes.

## Current limitations

The dedicated composition lifecycle now captures the volume snapshot during
profile setup and sets it before binding the composition. It deliberately does
not link through the legacy adapter runtime. That wiring has a source-build
record, but the current minimal Bochs recipe has not yet linked the expanded
composition closure; no native CLI volume-capability claim follows. Existing
mixed boot-namespace regression failure at its independent `50:11` assertion
is retained evidence, not proof about the volume provider.
