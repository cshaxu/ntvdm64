# OpenNT Artifact Matrix

## Evidence Scope

Status: reconciled against the current OpenNT-tools16 guest closure on
2026-08-08. Older WLink/OpenWatcom and source-built-MASM diagnostics retained
elsewhere are historical evidence only; they are not the active build state.

This is a source-location matrix for the checked-in OpenNT `base/mvdm`
snapshot, revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`. It does not certify that
this snapshot, its historical build configuration, or any resulting file is
bit-identical to a retail NT4 EN-US runtime.

The NTIO source closure is now a formally managed subset of that snapshot:
123 unmodified files under `src/opennt/base/mvdm/dos/v86/{doskrnl/bios,inc,messages}`
are hash-manifested upstream copies. Its compatibility-build stage must use
this managed root and verifies the closure before copying it to a disposable
work area. The COUNTRY closure likewise has 19 unmodified text-only files
under `src/opennt/base/mvdm/dos/v86/dev/country`; its stage separately verifies
the already-managed shared `inc/copyrigh.inc`. Other artifact families retain
their separately recorded source and toolchain states; these imports do not
promote their staged outputs.

`HISTORICAL-TOOLCHAIN-PROVENANCE-MATRIX.md` is the cross-artifact authority
for distinguishing the historical tool named by a build description from the
compatibility or reconstructed tool actually used by a research stage.

## Completeness Rule

The named files below are representative roles, not a scope limit. The build
scope is every one of the **75 declared outputs** in
`opennt-mvdm-artifact-inventory.json`, plus every output whose name must be
resolved from the corresponding makefile/install rule. Each record must be
classified as one of: `NT4 DOS payload`, `VDM host/support`, `WOW16 dependency`,
or `sample/development-only`; it must then carry a source-closure, toolchain,
link, packaging, and runtime-validation state. A file is not silently omitted
because it is not among the initial examples.

In particular, the payload set includes the system images, command processor,
all command/device utilities and NLS resources declared below `dos/v86`, the
redirector, DOSX/DPMI pieces, and the V86 helper programs. Keyboard, display,
mouse, comms, sound, printing, and other Win16 driver families are tracked as
WOW16 dependencies when their historical output is a `.DRV` rather than a DOS
`.SYS`/`.COM`/`.EXE`. Build descriptors under `vdd/samples` and `wow16/test`
remain tracked but are not promoted to a default runtime payload.

| Role | Observed source/build location | Observed output expression | BYOB role |
| --- | --- | --- | --- |
| I/O/bootstrap | `dos/v86/doskrnl/bios/makefile` | `ntio.sys` | managed-input source-to-image rebuild verified; bootstrap validation pending |
| DOS kernel | `dos/v86/doskrnl/dos/makefile` | `ntdos.sys` | managed-input source-to-image rebuild verified; runtime validation pending |
| Guest redirector TSR | `dos/v86/redir/makefile` | `redir.exe` | structural stage image; requires optional BOP 0x57 host service provider |
| Command processor | `dos/v86/cmd/command/makefile` | `command.com` | unchanged-source OpenNT MASM 4.00/LINK16 image rebuild verified; runtime/host-chain validation pending |
| DOS debugger | `dos/v86/cmd/debug/makefile` | `debug.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime validation pending |
| Line editor | `dos/v86/cmd/edlin/makefile` | `edlin.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime validation pending |
| EXE-to-binary utility | `dos/v86/cmd/exe2bin/makefile` | `exe2bin.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime validation pending |
| Graphics print-screen command | `dos/v86/cmd/graphics/makefile` | `graphics.com`, `graphics.pro` | `GRAPHICS.COM` is rebuilt with OpenNT tools16; `.PRO` is a static companion; runtime validation pending |
| XMS manager | `dos/v86/dev/himem/makefile` | `himem.sys` | unchanged-source OpenNT tools16 rebuild verified; original XMS/runtime validation pending |
| DOS extender | `dpmi/makefile`, `dosxi.lnk` | `dosx.exe` | unchanged-source OpenNT tools16 rebuild verified; DPMI/protected-mode runtime remains deferred |
| Redirector guest side | `dos/v86/redir/makefile` | `redir.exe` | optional/adapter research |
| Redirector host side | `vdmredir/sources` | `vdmredir` DLL target | optional/adapter research |
| Editor payload | `dos/v86/cmd/qbasic/makefile` | install rule names `edit.com` | optional research |
| Search-path utility | `dos/v86/cmd/append/makefile` | `append.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| File sharing utility | `dos/v86/cmd/share/makefile` | `share.exe` | unchanged-source OpenNT tools16 rebuild verified; intentional NT no-op semantics still require trace evidence |
| Version-table utility/device | `dos/v86/cmd/setver/makefile` | `setver.exe` | rebuilt with OpenNT tools16 plus the documented MS-DOS 4.0 C closure; runtime pending |
| High-memory load utility | `dos/v86/cmd/loadfix/makefile` | `loadfix.com` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| File-open cache utility | `dos/v86/cmd/fastopen/makefile` | `fastopen.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| Keyboard layout device | `dos/v86/dev/keyboard/makefile` | `keyboard.sys` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| NLS service utility | `dos/v86/cmd/nlsfunc/makefile` | `nlsfunc.exe` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| Memory reporting utility | `dos/v86/cmd/mem/makefile` | `mem.exe` | rebuilt with OpenNT tools16 plus the documented MS-DOS 4.0 C closure; runtime pending |
| Keyboard control command | `dos/v86/cmd/keyb/makefile` | `kb16.com` | unchanged-source OpenNT tools16 rebuild verified; monitor/runtime validation pending |
| ANSI console device | `dos/v86/dev/ansi/makefile` | `ansi.sys` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| Country/code-page database | `dos/v86/dev/country/makefile` | `country.sys` | unchanged-source OpenNT tools16 rebuild verified; runtime pending |
| WOW16 mouse driver | `wow16/drivers/mouse/makefile` | `mouse.drv` | separate Win16 driver family; unchanged-source OpenNT tools16 rebuild verified |
| V86 termination BOP utility | `v86/util/makefile` | `quit.com` | source-build candidate; monitor required |

## Build Implications

The direct payload closure now has 22 unchanged-source OpenNT-tools16 builds,
seven static companions, and four explicitly non-source-built `binplace`
inputs. `QUIT.COM` is also source-built, but remains a termination-BOP utility
rather than a normal first-profile input. Every built file still needs its own
runtime-load/contract validation; none is claimed retail-identical. See
`HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md` and
`NT4-GUEST-ARTIFACT-CLOSURE.md`.

`COUNTRY.SYS` has an isolated stage/link pipeline and a structurally plausible
stage image. Its source
unconditionally includes `copyrigh.inc`; the current complete OpenNT checkout
tracks that source input (Git blob `d854d5da283a2891c403ef29866f2018ac25baa1`).
The compatibility candidate is not a runtime-validated NLS payload. OpenNT's
mouse source instead produces the WOW16 `MOUSE.DRV` driver, so it is tracked as
a distinct Win16 family and not mislabeled as a DOS mouse system file.

`GRAPHICS.PRO` is an unchanged source-tree companion file. `GRAPHICS.COM` has
a current unchanged-source OpenNT-tools16 build record. Earlier OpenWatcom
WLink diagnostics for `grpattrn.obj` are retired comparison evidence, not the
active build state; runtime validation remains pending. See
`HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`.

`dosx-tools16-historical-v6` records the makefile's x86 output,
`486\DOSX.EXE`, built from unchanged source with the fixed OpenNT 4.5
MASM/LINK16 pair (36,152 bytes, SHA-256
`8F21AC29995DADBBD9AC9D8260FD61ED43AC832F4E4BF2F168381524D09916F7`).
This is source-build evidence only: it does not admit DPMI/protected-mode
runtime support.

The first five entries support the initial `nvtdm` identity profile because
they correspond to observed, separately built historical roles. They are not
proved to be the complete runtime: configuration, NLS/message resources,
redirector composition, toolchain version, and final packaging are external
inputs that must be recorded by an owner-supplied isolated build.

`VDMREDIR.DLL` is intentionally optional. The historical host-side redirector
is evidence that DOS namespace integration is a host contract; it is not a
reason for the owned engine to link historical code or private Windows APIs.

## Full-Set Delivery Gate

Producing the complete set is an iterative build campaign, not a claim that
one modern command can recreate a retail installation. For every declared
output, the final manifest must record: source revision and source closure,
EN-US resource inputs, exact compatibility-tool versions/commands, produced
file hash and byte count, unresolved imports or placement constraints, and a
runtime result. The compiled payload must remain in the local research output
area and is not a distributable artifact of this project.

`OPENNT-FULL-ARTIFACT-CLOSURE-WORK-ORDER.md` defines the campaign ordering for
this delivery gate. It keeps P0 command-smoke artifacts, toolchain-blocked
families, indirect descriptor outputs, and host/WOW pairings visible as one
full-set obligation rather than treating current structural stages as the
finish line.

Every final per-file record must also point to the applicable row in
`HISTORICAL-TOOLCHAIN-PROVENANCE-MATRIX.md`; an artifact may not inherit a
different artifact's assembler or linker evidence by family resemblance.

## Reproduction Gate

Before a historical build may be attempted, use
`tools/governance/Test-HistoricalBuildInputs.ps1` with owner-provided archives. The tool
only reports existence, SHA-256, and whether the requested build root is
outside this project. It neither extracts nor executes the archives.
