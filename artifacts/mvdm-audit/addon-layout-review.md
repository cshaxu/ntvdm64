# T473 S10 owner-directed add-on and tool cleanup

This stages the owner's final source names and output names. It is a
bounded producer/consumer cleanup within S10; it does not close the MVDM-wide
audit or T473. No OpenNT mirror, original guest or original disk image is
modified or rebuilt. The ten host images retain their existing hashes.

## Current P: Setup correction and closure gates

The owner adds the following finite requirements to this P, which must close
before the original S10 whole-source census resumes. The rename/publication
and fixture results below are intermediate evidence, not P closure.

1. Reconcile every src/addon and tools directory with its owner and active
   producer/consumer, retaining required user tools and accounting for archives.
2. Reproduce and identify the missing mouse in original graphical Win3.1 Setup.
   Trace Setup driver selection, the actually installed driver, INT33 delivery
   and the authored bridge before changing any provider. Original media remain
   read-only; use recoverable build-owned copies only.
3. After original Setup returns, SETUP.CMD may create only the installation's
   PATCH profiles/configuration and owned scratch. It must not patch or replace
   installed MOUSE.DRV, WIN386.EXE or KRNL386.EXE. Test their before/after hashes,
   including MOUSE.DRV, with repeat/failure/conflict preservation.
4. Execute all four DOSONLY=0/1 by Standard/386 Enhanced combinations. Bind
   observations to the actual package, mode and worker/module lineage; a
   startup witness alone does not prove absence of host WOW32 execution.
   Installed full Win3.1 must remain a DOS workload in both settings.
5. Do not introduce the future wow31/win31-wow kernel-conversion package here.
6. Complete required grouped build/test/publication, review the final diff,
   commit/push this P, then resume the paused S10 audit. Any unproved runtime
   row remains explicit and cannot be reported as passed.

Initial read-only findings: current SETUP.CMD already generates only profiles
after Setup; its existing fixture omits MOUSE.DRV from protected inputs. The
existing runtime matrix observes DOSONLY and Windows mode but does not inspect
WOW32 module lineage. Retail SETUP.INF offers several mouse drivers, so replacing
MOUSE.DR_ alone does not establish which driver graphical Setup selected. These
are test/attribution gaps, not yet established root causes of the user failures.

The retained real Setup text capture
`build/w31-setup-user-r1/r9-screen.txt` shows that this earlier run selected
"Microsoft, or IBM PS/2". Retail SETUP.INF maps that selection to mouse.drv;
the capture does not prove which driver was loaded during the graphical phase
or that input reached it. The earlier VFY9 installed tree is no longer present,
so its installed driver cannot be identified from that path. A new owned-copy
runtime observation is still required.

The post-Setup fixture now includes SYSTEM/MOUSE.DRV among its hash-protected
inputs. Its short-path run `build/t473-s10/addon-setup-profile-r10` passed repeat
and conflict cases using a Setup shim; an earlier long-path run r9 failed PIF
creation and is retained as a test-path failure. Neither is actual Setup proof.

The next isolated fixture `build/w31m12` is a copy of the supplied retail
media. Current APPLY succeeded (receipt `build/t473-s10/setup-media-r12`);
expanding its MOUSE.DR_ yielded the released MOUSE31.DRV SHA-256
`186AD08C4E75BA528001D29C102797B05901BCD74C720334D5859EB53CF390C7`.
The copied SETUP.INF matches the untouched original. This proves the selected
media input identity only, not runtime loading or mouse operation.

The first isolated runtime matrix (`build/t473-s10/w31-matrix-r13`) uses a
copy of the owner installation with freshly generated PIFs and the current
ten-image package. Standard DOSONLY=0 and 1 reached Program Manager with the
expected mode and returned normally. Enhanced DOSONLY=0 did not reach the
startup witness: the captured timeout desktop contains an NTVDM dialog saying
the application requested COM1 and the system cannot open it. This is a
failed case, not evidence of the cause of the reported WOW routing issue.
Enhanced DOSONLY=1 also timed out at the same COM1 dialog. Both cases retain
their individual DOSONLY witnesses and failure captures. Historical T436
evidence shows that ignoring only the named serial dialogs exposed a further
DOSMGR failure in an older package; it does not prove the present result.
Current-package continuation and process/module attribution remain pending.
The r17 rerun handles the identified serial dialogs and reports all four
DOSONLY/mode witnesses plus normal exit. It uses an installation copy with
regenerated PIFs having NT31.FLAGS=1. The owner's `O:\w31\PATCH\WINSTD.PIF`
instead has NT31.FLAGS=0; both copied and original kernel files have matching
hashes. OpenNT defines subsystem 1 as DOS and 0 as default; `cmdpif.c` uses
the former to skip DOS binary type checks. This is a significant difference
in test inputs, not proof that the owner's invocation enters WOW32. Reproduce
the original PIF setting and capture its actual worker/module lineage before
claiming the user's original launch is fixed.
The path-only relocated original PIF reproduced the reported failure in r19:
the private-desktop snapshot shows a `WOWExecClass` window and a "Can't run
16-bit Windows program" memory dialog, while its second NTVDM loaded
`WOW32.DLL`. DOSONLY=0 was independently witnessed. The r17 PIF differs in
CloseOnExit, XMS and 386 flags as well as subsystem, so r17 alone does not
identify which individual difference caused the routing. The original
installation was not changed.
Owner clarified this installation was freshly created with the setup tool.
Read-only inspection of the actual `O:\ewin31\PATCH\SETUP.CMD` confirms it
generates PIFs without any legacy XMS, 386 or NT31 options. Its SHA-256 is
E929AFFE69815FB7ACE3749A850E129E7284AA0050950F9E2F1AF710C4A525A6,
different from the current source script. Thus the deficient installed profile
was produced by the earlier setup delivery; the owner did not misconfigure it.
The r20 isolated single-field test changed only the relocated original PIF's
NT31 field to 1 and still reached a WOWExec memory dialog. Further attribution
and a tested recovery path for already prepared media/installations are needed.
Owner subsequently tested a fresh installation: Setup, Standard and Enhanced
all lack a usable mouse, while installation and startup succeed. Read-only
inspection finds SYSTEM.INI selects mouse.drv and the installed driver hash
matches the released MOUSE31.DRV. A diagnostic in an isolated installed copy
(`build/t473-s10/mouse-diag-r22`) observes Windows 3.10, a loaded driver,
allocated/installed/enabled callback and one completed synthetic DPMI call.
The first expected cursor coordinate failed: expected approximately 100,80;
USER returned 99,109. The fixed-coordinate probe assumes a specific geometry,
so this is not yet proof of the user's physical mouse failure. Its outer
matrix deliberately lacks the usual startup witness and is not an acceptance
result. Trace actual input and the provider/display geometry before a fix.

Subsequent controlled input/paint comparison: in `mouse-input-r24` the actual
frontend sink accepted movement and Win3.1 USER moved from (320,240) to
(418,272). The previous and current authored mouse drivers produced the same
result. In `mouse-surface-r28`, Video 7 produced no pixel changes for this
movement. With only the isolated display configuration/files switched to
retail standard VGA, `mouse-vga-r29` produced an arrow and 106 changed pixels.
This establishes a mode-dependent presentation gap, not a Raw Input/capture
test. The original V7 controller invokes `paint_v7ptr` and `clear_v7ptr`; the
selected NTVDM link map places both in `stubs.obj`, initialized to `dummy`.
Standalone SoftPC supplies the missing V7 cursor compositor in
`src/app-softpc/compat/v7_pointer.c` (SHA-256
`1C0F8893DAC407575D00D40191A50CEBF2E267E1C75539FFB59CCF8E78B1F5B2`).
Its surface lifetime generation, mask compositing, bounds and damage semantics
must be adapted to the NTVDM graphics owner, not replaced by guest patches.
The same SoftPC tree also parenthesizes the V7 pointer-enable comparison;
the matching one-line mirror correction is staged and requires compilation,
actual controller tests, cursor rendering and lifecycle regressions before use.
Owner independently confirms VGA has a cursor and V7VGA does not. SoftPC's
compositor requires a surface-generation identity, completed body readiness,
and a damage transaction; current NTVDM `ntvdm_console_graphics_invalidate`
validates a rectangle but marks the full capture dirty. Porting only the mask
loop or treating that API as SoftPC's dirty-rectangle transaction would leave
repaint, resize and first-frame correctness unproved. Its original 32x32 AND/XOR
mask semantics remain the reuse target; bind them to NTVDM's bitmap owner and
verify redraw, erase, surface recreation, publication and cross-mode behavior.
An independent direct-PIF diagnostic (`build/t473-s10/enhanced-dialog-r14`)
has instead reached an illegal-instruction dialog at 04d3:22c0 before recording
any serial-dialog action. Its invocation differs from the matrix's CMD wrapper;
this observation cannot establish the cause of either matrix failure. The
observation terminated with `result=timeout`; its outer runner exited zero,
which is not a successful guest result. No serial-dialog action was recorded.
Its working directory and environment differ from the installed CMD wrapper,
so it cannot substitute for the requested four-case acceptance.
The subsequent normal-CMD diagnostic (`enhanced-dialog-r16`) discovers the
system CMD intermediary and test-owned descendants. Its private-desktop log
records Ignore on the exact COM1, COM2 and COM4 warnings, followed by a visible
Windows 3.1 window. This is not yet a Program Manager, mode or orderly-exit
witness, and it does not turn either failed matrix row into a pass.
The dialog originates in `softpc.new/host/src/nt_com.c`: a failed exclusive
`CreateFile` of the configured COM device invokes `RcErrorBoxPrintf` once per
session. This identifies the immediate dialog path, not which guest operation
requested COM1 or whether the absent serial device is the sole startup issue.

## Final ownership

[src/addon](../../src/addon/README.md) now indexes **12 components**: nine
standalone AMD64 utilities and three independently authored guest add-ons.
The former names are replaced by mouse101, mouse31, compat95, patch31 and
path31. patch95 and path95 are separate source/build components. Each native
BUILD.CMD accepts one output path and keeps objects in an output directory.

The final renamed products are exactly **patch31.exe, path31.exe,
patch95.exe and path95.exe**. The other tools retain PIF.EXE, HASH.EXE,
GRP.EXE, LNK.EXE and SZDD.EXE; drivers retain MOUSE101.DRV, MOUSE31.DRV and
compat95.sys. All 12 outputs are in assets/release and the project system32,
with the one add-on manifest recording source and output identities.

The two Win95 native tools share only common/file-helpers.h, a small
project-owned static file reader. Neither component reaches into the other's
private directory. The patch95 MIT decoder notice remains with patch95;
path95 remains an independently authored public-format implementation.
compat95's provenance retains the previous source snapshot and records the
current renamed source/hash instead of retaining a stale current binding.

The existing tools/win101-setup scripts were newer than the duplicated four
files under src/addon. Their paths, CloseOnExit and local TEMP handling were
preserved. The old source copies were removed; unique recovery guidance was
carried into the canonical README. No newer tool was overwritten by an old
script during the consolidation.

## Producer and consumer fixes

- Corrected every active old source/file reference, including five authored
  mouse-harness includes, both mouse producers and both driver stagers.
- Corrected compat95's assembly input, build manifest and release binding.
- Split the two Win95 BUILD outputs into independent single-output entries;
  moved their shared reader to a neutral header rather than duplicating it.
- Native staging now validates source identity before copying and replaces
  prior entries by ID, output or source identity, preventing duplicate owners
  after a rename. No public compatibility alias is added for retired names.
- Operator scripts consume released native tools and compat95 rather than
  carrying a second copy. PIF/HASH usage documents live with source; their
  unused tool-directory EXEs and directories are removed.
- Removed unreachable old Win3.1 bodies after exit /b in Win95 path wrappers.
  The live Win95 command path is unchanged except for its released tool name.
- Fixed Win1.01 producer output-root handling for absolute paths. HASH and
  patch95 build scripts now keep objects beside outputs, rather than the
  caller's directory. Relevant CMD test children explicitly launch hidden.

## Actual build and test evidence

The [retained receipts](addon-layout/manifest.json) contain exact source
hashes, commands, owned process IDs, terminal outcomes and logs. The final
build has **23 successful steps**: twelve component builds and eleven tool
groups. All three driver outputs are byte-identical to their preceding
releases, so prior acceptance labels are retained, including compat95's
candidate label. Native tools are freshly built from the final source homes.
Publication checks all **24 destination hashes**. The unchanged ten-host
package retains its previously passed selected Full evidence; these offline
tools are not host link inputs.

The functional groups exercise Win3.1 discovery/apply/repeat/exact recovery,
Win1.01 setup orchestration/cleanup, Win3.1 compressed-media roundtrip,
Win95 launch refusal/rollback and installation journals, alternate-root
Start Menu PIF provenance, PIF/HASH, PIF root updates, LNK metadata/encoding,
GRP root/checksum behavior and SZDD exact expansion/failure preservation.
All modifications are to constructed or build-owned copies. No actual Setup
or guest boot is claimed. Independently authored driver harnesses are
assembled and structurally checked; no new interactive driver/Win95 runtime
acceptance is claimed from source relocation.

Additional final-build GRP controls cover both a tagless original MAIN.GRP
and an augmented Win3.1 tag/working-directory fixture. Both pass, with 0 and
33 input tag bytes respectively; malformed PMCC still fails and checksums
remain checked. SZDD additionally passes empty/binary/block-boundary cases
and refusal to overwrite or produce output from missing input.

## Failures preserved and corrected

Earlier receipts are retained as failures, never rewritten to green:

1. The provisional w31path build path disappeared while the owner performed
   the broader rename. The owner confirmed the new tree was authoritative;
   final compilation uses path31 rather than recreating the old directory.
2. Win1.01 producer rejected an absolute output because it blindly joined it
   to the repository. The producer now normalizes rooted and relative input.
3. An overlong Win95 test directory correctly hit the existing PIF OEM field
   limit. The test uses a short build-owned root; no product limit is relaxed.
4. GRP's old test assumed all MAIN.GRP inputs had a tag suffix. The real
   source here is tagless; an empty PowerShell range then fabricated bytes.
   Empty suffixes are now represented explicitly and must remain empty.
   Canonical tagged fixtures still retain strict header/directory checks.
   The intermediate null-array test mistake is retained and corrected too.
5. Splitting path95 exposed its shared header under patch95. Both now include
   the neutral common header and compile independently.

The [delivery verifier](addon-layout-verification.ps1) binds current source,
the full final tool matrix, driver equivalence, final names, deployment and
absence of retired aliases/duplicate homes. The separate
[tools review](tool-directory-review.md) gives retention/archive decisions.
Two console transcripts have trailing spaces removed for text hygiene; their
unmodified originals are retained in `addon-layout/final/raw-console-outputs.zip`
and both forms are covered by the evidence manifest.
It recommends dependency-aware archival of obsolete engineering closures;
it does not claim those historical directories have already moved.

The add-on reorganization adds no MVDM mirror difference or guest-source
exception. The separate [Video7 pointer correction](video7-pointer-review.md)
records its own two source-mirror divergences and test boundaries.
MVDM reconciliation remains at 1,538 pending, 450 partial, 53 full and 2,643
excluded physical file roles. The separate host-tail audit remains work in
progress and is not used as evidence that additional files are reconciled.
