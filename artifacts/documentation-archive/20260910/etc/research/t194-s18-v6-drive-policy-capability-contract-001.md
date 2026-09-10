# T194 S18 v6 drive-policy capability contract 001

Date: 2026-08-12  
Packet: M0 T194 S18  
Disposition: contract resolved; one contained adapter-provider integration is
the next candidate.

## Existing facts

The owner's CLI contract is already implemented at the runner/adapter seam:
`--include-drives` chooses the initially eligible drive letters,
`--exclude-drives` removes letters afterward and wins conflicts, and absence of
both selects every currently present logical drive. The runner passes only
child-only `NTDOS64_HOST_INCLUDE_DRIVES` and `NTDOS64_HOST_EXCLUDE_DRIVES`.

At adapter-session installation, `bx_ntvdm_host_drive_snapshot_v1_capture_from_environment`
performs one `GetLogicalDrives`/`GetDriveTypeW` capture, applies that policy,
and stores only the 26 drive letter/type values. It opens no paths, is not a
Bochs ABI and cannot be called by guest code. The snapshot is immutable for
the session. This is the existing T95 S2 capability contract.

There is a separate legacy `host_drive_inventory` JSON member. It is a static
profile value, not influenced by CLI include/exclude arguments. Current runtime
uses this static member for `50:0F`, while `S13` has no such member even though
it correctly installs the live CLI snapshot. That split produces the S16
unserved `50:0F` outcome and violates the owner's rule that CLI arguments
control drive visibility.

## Resolved ownership and data contract

For a normal CLI launch, the one existing immutable
`bx_ntvdm_host_drive_snapshot_v1` is the sole `50:0F` input:

1. Runner normalizes and transmits include/exclude policy to its one engine
   child.
2. Adapter installation performs the only host enumeration before guest
   execution, then freezes the 26-slot drive-type snapshot.
3. The already source-derived `demGetDrives` provider consumes that snapshot,
   preserves OpenNT's contiguous-drive counting rule, writes AX and clears CF,
   and resumes after the four BOP bytes.

An absent policy flag therefore exposes all present drives *as finite drive
types only*; an include set restricts that finite set; exclusion then wins.
This grants neither a DOS pathname, file handle, directory search, host root,
mount, DPB device, nor a later live query. `50:46` and host namespace access
remain separately gated and are not admitted by this contract.

The static profile `host_drive_inventory` must not override or be merged with
the CLI snapshot in this normal-launch path. Retained historical/static
fixtures may continue to use it only where their explicitly declared fixture
contract requires it; they are not the CLI policy source.

## Next implementation boundary

T194 S19 may make the smallest adapter-only selection change: dispatch the
existing `bx_ntvdm_dem_drive_provider_v1` with the already installed immutable
snapshot for `50:0F`, and add positive/negative tests for default, include,
exclude and conflict snapshots. It must not add a new BOP, call the host during
a guest request, change Bochs, implement filesystem access, or create a new
v6 root/run in the same S.
