# M0 T194 S25: Immutable Drive-Snapshot DPB Route

Packet: M0 T194 S25  
Disposition: adapter-only source and focused-test closure.

## Question

Can the reached OpenNT `DemGetDPBList` (`50:46`) use the already admitted
normal-launch drive snapshot without allowing the obsolete profile-static
inventory to override it?

## Change

`bx_ntvdm_adapter_runtime_v1_dispatch_dem_dpb_snapshot` now classifies the
copied BOP through the existing ingress, provider registry, and DEM-plane
records. For only classified `50:46`, it passes the immutable snapshot types
to the pre-existing `bx_ntvdm_dem_dpb_service_v1_prepare` producer and queues
the pre-existing opaque multi-write transaction.

When that snapshot contains no removable or fixed drive, the selection gate
consumes the snapshot decision but leaves the normal pass-through result and
queues no transaction. This prevents a retained `host_drive_inventory` JSON
entry from becoming an implicit fallback. It does not claim the original
empty-list resume contract, which requires a later generic ABI decision.

## Focused Verification

The configured MinGW target rebuilt only
`bx-ntvdm-adapter-runtime-test` in
`artifacts/build/current/t194-s15-cli-v6-mingw-r1`. Both commands exited zero:

```text
bx-ntvdm-adapter-runtime-test.exe --t194-drive-snapshot-routing
bx-ntvdm-adapter-runtime-test.exe --t194-drive-snapshot-dpb-routing
```

The new second mode creates an isolated v6 profile whose static inventory says
C is fixed. Its positive half admits only live C, invokes `C4 C4 50 46` with
ES:BP `0100:0200`, and proves the queued result resumes at `0x7c04`, advances
BP to `0x0221`, and contains exactly two writes at physical `0x1200` and
`0x1219`. The payload is the source-limited C drive/unit pair followed by the
terminal `Next = -1`.

Its negative half excludes every drive while retaining that same static C
fixture. It proves no multi-write is pending and the result remains
pass-through. Thus the live empty snapshot wins over the static fixture and no
host path, namespace, device, filesystem, Bochs code, or guest process is
used.

## Limits And Follow-up

This is a route to an existing source-derived producer, not a new DPB
implementation. It does not run the Bochs engine or assert that the real guest
will continue after `50:46`. The next bounded slice is a source-only current
adapter derivative build, preserving the existing Bochs object closure; an
engine bundle or observation needs a later S packet.
