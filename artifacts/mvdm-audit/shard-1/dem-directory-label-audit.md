# Original DEM directory, label and diagnostic ownership

The frozen sources `dos/dem/demdir.c`, `demlabel.c`, and `demmsg.c` match
OpenNT byte-for-byte. Current selected x86 worker build lists each source in
`original-mvdm-dem.lib`. Worker map SHA-256
`641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`;
worker image SHA-256
`D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`.
Map ownership and behavior are distinguished below.

## Directory

`demdisp.c`'s SVC table dispatches create, delete, query-current and
set-current to the selected `demdir.obj` functions. `demCreateDir` and
`demDeleteDir` pass guest paths to OEM wrappers and propagate errors through
`demClientError`; `demQueryCurrentDir` verifies the drive and CDS path,
resetting an invalid path and associated `=X:` variable to drive root;
`demSetCurrentDir` updates host current directory then the drive-specific
variable. Creation with nonzero BX/SI takes the original extended-attribute
early branch, without claiming completed EA support. Source call and map
identity do not establish guest-pointer lifetime or all edge-case behavior.

Historical T420 S33 `docs/etc/evidence/m0-t420-s33-dem-capability-closure.md`
records actual guest create/remove directory and selected ordinary file
operations. T420 S37 `docs/etc/evidence/m0-t420-s37-oemuni-capability-review.md`
records non-ASCII create/chdir/remove and a COMMAND `=X:` readback on its
specified historical product, with exact environment/path comparison. Those
witnesses have different historical binary hashes and do not prove this S10
snapshot or all errors, EA cases, missing drives and nested/reentrant lifetime.
The T430 S5 directory reset fixture concerns `demsrch.c` and must not be
misattributed to `demdir.c`. Maintain pending until the original function
and guest wiring have current-identity integration and negative coverage.

## Volume label

`demfile.c::demCreateCommon` dispatches create with `ATTR_VOLUME_ID` to
`demCreateLabel` after obtaining a bounded guest-path copy; `demfcb.c` has
separate FCB create/delete label routes. The selected map attributes the
create/delete label routines to `demlabel.obj`. They delegate to host
`SetVolumeLabelA` and map boolean failure to 1; real volume mutation must not
be performed on any user volume as an audit shortcut. A historical T230
component fixture `t230_s3_demfile_direct_import_fixture.c` uses an invalid
`?:LABEL` path to test a label failure, but it is not evidence of success on
real media. T420 S37's later GetMediaId volume query and padding test is a
read-only query, not evidence of create/delete label semantics.

Initial source review identified a read-boundary concern (the direct-object test below subsequently confirms the body-level read):
`demCreateLabel` loops on the pointer `while(s)`, increments the pointer, and
breaks after its length counter exceeds 11; it does not stop on NUL. Whether a
reachable caller can provide a sufficiently short accessible guest buffer or a
path whose end lies at a protection boundary, and whether resulting bytes
change observable behavior, has not been established. This is original source,
not proof of a new divergence, crash, or vulnerability. The subsequent guarded fixture stops before the volume-label API; real caller reachability remains pending.

## Debug diagnostic

`demmsg.c` defines `demPrintMsg` and message array only under `#if DBG`;
`dem.h` defines `demPrintMsg(x)` to no-op in the other branch. The release
worker graph contains `demmsg.obj`, but inspection of that selected object
and the release map shows no `_demPrintMsg` definition. Therefore mere
compiled-object membership does not establish debug diagnostic functionality
in this release. This is consistent with a conditional debug-only profile,
not evidence that message handling is fully restored or approved for removal.
Verify the intended profile and any separately selected debug build before
classifying. No new runtime or user-media tests were run in this audit.

### Bounded source-level candidate confirmed at selected object boundary

An offline 32-bit fixture directly linked a copy of the actual selected
`demlabel.obj` after removing only `.drectve`. It created two pages, protected
the second against access, placed caller-shaped `?:\\A\\0` at the end of
the first page, and invoked `demCreateLabel(path[0], path+3)`. A vectored
exception handler accepted only a read access violation at the first byte of
the guard page and at `demCreateLabel+0x40`, the selected object's input-byte
load. The program returned zero only upon observing that exact fault:

```text
PASS bounded label: selected object read exactly one byte beyond NUL into guarded page, before volume call
fixture-exit=0
```

This confirms the direct original function reads beyond a short NUL-terminated
label when the caller provides only that accessible input. The test did not
change a volume: the fault occurred before `OemToCharA` or
`SetVolumeLabelA`; `?` is an invalid drive fallback even if the expected fault
were absent. Fixture supplies simple security-cookie/range-failure boundaries;
it is not the production executable or guest address-space layout. Source and
selected object are original; this is not a new mirror divergence. The test
does **not** prove a real DOS caller can position a label immediately before
an inaccessible page, or that the guest path produces a reproducible failure.
`demCreateCommon` uses a local 260-byte bounded path copy, while
`demFCBCommon` obtains a `GetVDMAddr` alias; their actual reachable lengths,
alias layout and lifetime require separate source/integration proof. Keep
label row pending and do not call this a confirmed user-visible defect.

Fixture `dem-label-guard-probe.c` SHA-256
`964537EE0181D4DA5CAEE683E9CD94F9FCC9A14FE6EA47680742BF77F074C8A2`;
fixture exe SHA-256
`A1128BB59E7D0999242B9BB583F08F4BF13B5A5A62DD7A31EB1E9313777D6935`;
selected original object SHA-256
`C6405450FFB6674BF4E91C1BE6B3AC02809B22E84E91A034873E61557E09D1C6`;
stripped link copy SHA-256
`83B64EEEB852D0398AA0BD0A7F2E71D429E1E7D5FA73F99FF706EE05E043C774`.
No NTSRV, NTVDM or guest was started or modified.

### FCB caller shape follow-up

The original normal FCB open/create path in `doskrnl/dos/fcbio2.asm` calls
`TransFCB` with `OpenBuf` (declared 128 bytes), then sets DS to DOSDATA and
SI to `WFP_START` before `SVC_DEMCREATEFCB`; the associated DOS source
`dir2.asm` describes WFP as a NUL-terminated canonical path beginning with a
three-character drive prefix. `demCreateFCB -> demFCBCommon` passes that
translated name's byte 0 and offset 3 to `demCreateLabel` for volume-label
attribute 8. This is a DOS-kernel-generated pathname in the normal FCB route,
not evidence that an ordinary caller can pass an arbitrary raw pointer right
up to a host guard page. A direct BOP caller or other guest modification is
not assumed equivalent to this ordinary route.

The selected map binds `Sim32pGetVDMPointer` to `host/src/sim32.c`. That
function converts the guest address, returns `GetVideoMemory` for video, or
`NtGetPtrToLinAddrByte` otherwise; the source warns about its pointer backing.
This establishes the original source path, not the address/guard guarantees
of each deployed backing or safe cross-page lifetime. No actual FCB volume
operation or guest test was performed here. The object-level NUL overread is
real for a tightly bounded direct input, while its normal product reachability
and effect remain unproved.
