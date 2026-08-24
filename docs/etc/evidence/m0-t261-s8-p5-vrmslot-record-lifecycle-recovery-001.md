# M0 T261 S8 P5 — `vrmslot.c` record lifecycle recovery

## Question

Can the reached VDMREDIR mailslot group replace its project-owned fixed array
with OpenNT's `VR_MAILSLOT_INFO` lifetime without creating another guest-handle
allocator or exposing a native `HANDLE` to the guest?

## Source baseline

- `refs/opennt/base/mvdm/vdmredir/vrmslot.c`: `VR_MAILSLOT_INFO`,
  `VrpAllocateMailslotStructure`, link/unlink/map and process-cleanup helpers.
- `refs/opennt/base/mvdm/inc/vrmslot.h`: record layout and helper contract.
- `src/adapter-softpc/bx_ntvdm_host_handle_manager.c`: the sole session-owned
  opaque host-handle map.

## Recovery

`src/opennt-bop/vdmredir/vrmslot.c` now owns the original-shaped record list,
lookup and teardown lifetime.  `57:0B/09/0A/0D/0E/0F` consume only the record's
16-bit opaque token and resolve its native handle through the existing Direct
host-handle callback.

`BOP-DIV-058` records the one required change: OpenNT's private
`Handle16Bitmap` is not retained.  The one session-owned manager allocates a
monotonic opaque token; a token is range-checked before it enters a mailslot
record.  This prevents a second handle allocator while preserving the original
record/lifecycle ownership.

The no-guest-buffer `VrTerminateMailslots` body is also now directly retained.
Its original cleanup ordering is unchanged; `BOP-DIV-059` resolves the
record's token with the existing CCPU/SAS handle facade, while `BOP-DIV-060`
supplies the same PDB value from the copied `AX` frame.  Unlike the old
composition route, this body does not fabricate an `AX=0` result.

`VrGetMailslotInfo` is directly retained as well. Its sole host-side
dependency, the original `VrpMapLastError`/`VrpMapDosError` utility, is
re-rooted under `opennt-host/vdmredir/vrputil.c`; no BOP-local error mapper was
introduced.

## Verification

Fresh outside-sandbox MSVC x64 `/MT`, CPU5/P-MMX Ninja graph:

```text
build/t261/s8-r011
ninja -C build/t261/s8-r011 -j 4 bin/t251-s3-redir-ingress-fixture.exe
T251 S4 Redirector: typed selector-57 lifecycle and mailslot owner group pass
```

The fixture covers create, information query, write, original `57:0C`
`ERROR_NOT_SUPPORTED` peek, read, delete, second create, process termination
and invalid-token failure.  It consumes returned opaque IDs rather than
assuming the original private allocator's first/reused numerical values.
It also proves that `57:0F` resumes with clear CF and leaves AX unwritten, as
the retained OpenNT cleanup helper does.

The fresh `r012` graph added the re-rooted `vrputil.c`, completed **314/314**
actions and reran the same fixture successfully.

## Boundary

This recovers the original record lifetime, information and simple termination
bodies, not every original `Vr*` function body.  The guest-frame copy, checked
guest-RAM copy and public Win32 calls remain in the BOP-owned composition file
until each individual source body can be routed through the existing
`adapter-softpc` CCPU/SAS facade.  No new BOP, adapter, mapper or Bochs
behavior is introduced.
