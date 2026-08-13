# T198 S19 First NTDOS Guest-Entry Map 001

## Question

What is the smallest faithful, four-layer transaction needed when original
NTIO first requests NTDOS, and which historical inputs may be reused without
pulling the unavailable NT4 V86/CCPU product composition into the modern
runtime?

## Original Call And Guest ABI

`src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm:247-252` loads
`dos_load_seg` into `DI`, emits `SVC SVC_DEMLOADDOS`, and then jumps to
`sysinit`; its own comment says failure never returns.  `dossvc.h:109` defines
that service as `0x11`.  The matching built guest form is therefore
`C4 C4 50 11`.

The historical host route is:

```text
NTIO init
  -> BOP 50 / service 11
  -> BIOS[50h] -> MS_bop_0
  -> DemDispatch(0x11) -> demLoadDos
```

`nt_bop.c:146-160` reads the service byte, invokes `DemDispatch`, increments
IP by the service byte, and runs historical idle policy.  A modern `bx-vdm`
ingress already receives the complete copied four-byte form, so its only
faithful success result is a typed resume at `fault_rip + 4`.  It must not call
`MS_bop_0`, recreate `BIOS[]`, or create a local general DEM dispatcher.

## Original Provider And Rejection

`demmisc.c:152-207` documents and implements `demLoadDos`:

- it computes destination from `GetVDMAddr(getDI(), 0)`;
- appends `\\ntdos.sys` to mutable historical DOS-directory state;
- opens it with `CreateFileOem`;
- repeatedly reads 16 KiB directly into the CCPU/SAS guest pointer; and
- calls `TerminateVDM` on open/read failure.

The source makes the intended meaning clear but is not an independently
composable provider.  `DemDispatch` materializes all 73 historical DEM
function addresses, while `demLoadDos` itself binds mutable path state,
Win32 file/HANDLE APIs, CCPU/SAS pointer reconstruction and historical VDM
termination.  Direct linking would violate the selected x64 `/MT` composition
and the adapter's no-ambient-host/no-CCPU boundary.  The historical route is
therefore **source evidence, not a runtime-link input**.

The retained `src/bx-vdm/bx_ntvdm_host_service_plane.c` expresses the same
already-derived shape (`low16(EDI) << 4`, identity-checked same-island NTDOS
bytes and a preflighted transaction).  It belongs to a superseded parallel
runtime composition and is likewise evidence only until it is reconciled with
the current `bx_ntvdm_boot_namespace_composition_v1`; it must not be linked as
a second adapter/session plane.

## Guest Artifact Classification

`NTDOS.SYS` is a hard first-profile BYOB role, not a host path lookup.

| Fact | Evidence | Classification |
| --- | --- | --- |
| Source/build origin | `dos/v86/doskrnl/dos/makefile` declares `dest=ntdos.sys`; `NT4-GUEST-ARTIFACT-CLOSURE.md` records stage `ntdos-tools16-historical-v1` | Source-built guest artifact |
| Bootstrap role | `COMMAND-ARTIFACT-ROLE-RECONCILIATION.md` names `NTDOS.SYS` a hard BYOB runtime image | Immutable profile input |
| Fixed research identity | SHA-256 `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`, 27,858 bytes | Profile-specific evidence, not a universal constant |
| Observed matching layout | live `DI=08B0h` yields physical `0x8B00`; image interval `[0x8B00,0xF7D2)` | Historical matching-pair observation only |

The provider must validate the selected NTDOS bytes against the profile
identity before it accepts a request.  The observed `08B0h` value is not a
provider constant: on every request, destination is exactly
`(uint64_t)low16(DI) << 4`.

## Admissible Provider Boundary

The next implementation may add one `bx-vdm` **DEM load-DOS provider module**
to the existing global ingress/plane/session structure.  It has precisely the
following transaction:

| Stage | Owner | Required behavior |
| --- | --- | --- |
| Recognize | Existing `bx-vdm` BOP ingress/catalogue | Exact real-mode copied `C4 C4 50 11`; no Bochs selector knowledge |
| Authorize input | OpenNT/profile boundary selected by `bx-vdm` | Immutable identity-checked NTDOS bytes; no path, HANDLE or ambient lookup |
| Prepare | `bx-vdm` provider | Derive `DI:0000`, preflight the entire payload against the one-MiB ordinary-RAM aperture, create one checked write action |
| Commit | `bx-mantle` / `bx-core` mechanical contract | Perform the all-or-nothing ordinary-RAM write; no DOS/BOP meaning in Bochs |
| Succeed | `bx-vdm` | Typed resume at the copied BOP's `fault_rip + 4` |
| Fail | `bx-vdm` to generic mechanics | Controlled VDM stop/explicit rejected disposition, matching historical non-return without fabricating a DOS error result |

The `16 KiB` historical file-read loop is not part of the adapter ABI: bytes
are already a bounded, immutable profile input.  Replacing it with a single
preflighted write preserves the externally relevant all-or-nothing guest
publication while removing the unavailable host-file mechanism.

## Explicitly Deferred After Success

The next original guest boundary after resume is NTIO/NTDOS `C4 C4 12`, a
distinct machine BOP for memory-size behavior.  It is neither DEM `50:11` nor
an implied result of loading NTDOS.  The provider admission must therefore
stop after proving exact NTDOS publication and post-BOP guest continuation;
it may not attach a synthetic BIOS result, DOS initialization, filesystem,
or COMMAND launch.

## Conclusion

The input, destination rule, publication atomicity, success continuation and
historical failure meaning are all source-evidenced.  The only unavailable
part is the old NT host composition, which has a bounded replacement boundary.
S20 may implement that one provider as a member of the current global
boot/guest-entry plane, with a source-built x64 `/MT` closure and native
fixture.  It must first show that it reuses the single existing adapter
session rather than reviving `adapter_runtime` or the legacy host-service
plane.
