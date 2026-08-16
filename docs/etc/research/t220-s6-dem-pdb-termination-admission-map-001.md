# T220 S6: DEM PDB-termination admission map

## Question

May the S5 direct-token `release_owner(PDB)` primitive be connected to DEM
`50:3C` while preserving OpenNT's process-termination order and the current
composition's bounded ownership boundaries?

## Inputs

- OpenNT `v86/doskrnl/dos/msctrlc.asm`, `abort.asm`, `dem/demsrch.c`, and
  `dem/demfile.c`.
- Current `bx_ntvdm_dem_session_lifecycle_provider_v1`, DEM package session,
  direct handle partition, file session, and boot namespace provider.
- T220 S1--S5 records.

## Original route and ordering

`msctrlc.asm` reaches `SVC_PDBTERMINATE` only after all three guest-DOS guards
hold: `PDB_Parent_PID != CurrentPDB`, `CurrentPDB == ThisPDB`, and the exit is
not `Exit_keep_process`. `BX` then identifies the genuinely removed child.

The resulting order is material:

```text
SVC_PDBTERMINATE(BX=PDB)
  -> demTerminatePDB: VDDTerminateUserHook (when applicable)
  -> HostTerminatePDB
  -> release per-PSP FindFirst/FindNext list
  -> arena_free_process
  -> DOS_ABORT
       -> guest JFT close loop
       -> FCB/SFT owner cleanup
       -> SVC_DEMCLOSE for each DOS SFT NT handle
```

`DOS_ABORT` explicitly owns the later handle/FCB/SFT teardown. In particular,
its `$close` loop and `Close_NT_Handle` issue `SVC_DEMCLOSE` after the
termination notification, not during it.

## Current composition route

The existing lifecycle provider accepts `50:3C` only through common ingress,
the original-precedence DEM provider selection, and the DEM MISC/deferred
plane. It invokes only `boot_namespace_provider::terminate_pdb(BX)`, then
returns the original VOID-style `RIP + 4` result without GPR/flag writes.
That finite boot-state action is separate from the current direct namespace
file-session held by `bx_ntvdm_dem_package_session_v1::whole_provider`.

The direct handle partition already implements ordinary `50:02` token close.
It resolves exactly one opaque token and calls `file_session::release(token)`.
S5 records the copied PDB for direct open/create tokens but intentionally does
not create a guest JFT/SFT representation.

## Disposition

**Reject a `50:3C -> release_owner(BX)` binding.** It would close direct host
handles before guest DOS reaches `DOS_ABORT` and its JFT/SFT `$close` path,
thereby changing ordering and allowing the later original close route to see a
forged/stale token. It would also incorrectly claim that S5's direct tokens
represent FCB, search, VDD, device, Redirector, or host-device lifecycle
state.

The retained `release_owner` primitive is not dead code: it is a future
compatibility seam only after a source-backed model proves the direct token's
relationship to guest JFT/SFT allocation and the exact post-`DOS_ABORT`
release point. That work requires a separately admitted bounded layout map;
it cannot be smuggled into `50:3C`.

## Failure and profile behavior

- A non-child, parent, mismatched, or TSR/keep-process exit does not issue
  `50:3C`; the adapter must not manufacture cleanup from an observed process
  event.
- The existing `50:3C` finite boot-state provider retains no-result resume;
  no failed `CloseHandle` is observable through it.
- Direct/readonly/overlay/virtual mutation choice remains orthogonal to the
  guest's child-termination guard. This map neither changes profile behavior
  nor enables host mutation.

## Interpretation and follow-up

Confidence is high: the ordering is explicit in the original guest source and
the current provider/session separation is direct in source. No runtime was
run. S6 closes by rejecting the tempting but semantically incorrect binding.

The next admissible investigation is a bounded JFT/SFT representation map for
the current direct tokens. It must define the exact guest-owned allocation and
post-`DOS_ABORT` release point before any lifecycle implementation is proposed.
