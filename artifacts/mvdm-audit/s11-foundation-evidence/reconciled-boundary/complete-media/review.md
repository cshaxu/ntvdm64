# Reconciled candidate with guest archive files

Compared all 62 `files` entries in `assets/winnt-guest-manifest.json` with
the prior reconciled candidate. Forty-five matched, fifteen archive binaries
were absent, and `system32/autoexec.nt` plus `system32/config.nt` differed.
The latter are configuration files; this comparison does not establish that
their differences are accidental or authorize replacing them.

A separate build-owned candidate was made by copying that candidate, then
extracting only the fifteen missing entries from `assets/winnt.zip` and
verifying each against the approved manifest hash. No existing file was
overwritten; no guest binary was modified or rebuilt. All sixty binary/data
entries other than the two configurations now match the manifest, as listed
in `guest-comparison.csv`. The ten-image product manifest is copied unchanged;
it binds only those product images, not guest-media completeness.

The existing isolated SOL Options observer then passed three open, close and
recovery rounds and normal launcher completion on this candidate; `probe.txt`
and `result.txt` retain the output. This is not a Win2/WIN87EM load, a Full
product run, or acceptance of either configuration difference. The preceding
candidate's tests are not automatically tests of this larger media set. No
user installation was modified or product package published.

`Stage-System32ProductPackage.ps1` copies the supplied baseline and replaces
the explicitly selected host images; it does not reconcile that baseline
against `assets/winnt.zip`. `New-PhysicalRuntimeStage` likewise copies the
candidate tree, rather than adding missing guest media. Thus a host-image
manifest and passing host tests cannot by themselves prove guest completeness.
Historical integration records explicitly staged from an extracted archive;
the incomplete baseline used here does not establish why the user's current
installation is missing these files. The complete-media copy is a test input,
not an installation or a published package.

The Product and Full test harness now invokes `verify-guest-media.py` before
running product cases. It checks the original manifest's immutable files by
hash and deliberately excludes the three configurable INI/NT files. The
complete-media stage passes 59 immutable inputs; the earlier stage is rejected
for its 15 missing entries, and a disposable synthetic mismatched file is also
rejected. `guest-media-gate/` retains direct-check output and input hashes.
The configurable files must still exist, but their contents are not hashed;
synthetic missing and differently configured files respectively fail and pass.
The PowerShell harness parses successfully; the new integrated Full gate was
not run, and Control deliberately remains independent of guest completeness.
An integrated Product negative on the deliberately incomplete stage stopped
at `guest-media` with all 15 missing names before any product case; a harmless
`where.exe` observer was selected so even an unexpected later path could not
launch a product. `guest-media-gate/product-negative/` retains its timing,
failure output and input hashes. This proves the integrated rejection path,
not the integrated Full success path.

On 2026-10-29, text comparison after normalizing CRLF showed both candidate
configuration files contain their entire archive versions as a prefix. The
only suffixes are comments: `REM Keep the Windows 1.01 SETVER table configured
for the next VDM start.` and `REM Windows 1.01 DOS version compatibility.`
This identifies the observed text difference; it does not assess any other
configuration source selected by a PIF or another launch.

The complete-media candidate also passed the isolated Write normal close,
restart and second close test; results and actual staged executable hashes are
in `write-lifecycle/`. This is not full WOW or guest-media validation.

The selected Control suite subsequently passed on this complete-media
candidate, including export ordinals, RPC, GUI startup, version rejection,
real EDIT, nested DOS/native handoff, repeated relaunch, frontend loss and
broker I/O. The runtime image manifest, gate timings and ordinal output are in
`control/`. Its first attempt stopped during fixture setup because the supplied
cache omitted the four test executables; it did not execute the product gates
and is not counted. The successful second attempt supplied the existing built
fixtures in a separate cache. Control does not include the WOW game frontier
and does not constitute the Full suite.

The same complete-media candidate separately passed the existing Console-17
and Window-17 command matrices. Their summaries, staged product manifest and
hashes are in `product-matrix/`; this complements Control but does not cover
the missing Winmine frontier or make this a Full suite result.

The complete-media candidate also passed the isolated SOL gameplay probe:
stock draw, Deal clearing a nonempty waste, a second draw, normal owned-window
close and launcher completion. Logs and staged input hashes are in
`sol-gameplay/`; no claim is made about other card moves or Winmine.

The previously built, hash-checked idle registrar and common-dialog Win16
fixtures also passed together in one actual worker using a fresh test package
identity. The guest reported 17 dialog creations, 16 explicit closes and
later idle delivery after the other task exited; both tasks retired. Binary
checkpoints, trace and staged manifest are in `idle-dialog/`. This does not
prove arbitrary cross-task interleavings or the real DLL attach-failure path.

On this complete-media candidate, the test-only VDD also resolved the two
original memory exports by name and ordinal inside a real NTVDM worker,
exercised query, allocation, reallocation and free, and checked that one
oversized reallocation failure preserved the original allocation and free
space. Original MEM completed. `export-live/` retains the trace, VDD hashes
and fresh staged product manifest. This is not all allocator failure modes.

The original WRITE startup-frontier verifier also passed on the complete-media
candidate: four samples retained one live worker and the visible menu, document
and page-info windows. `write-frontier/` retains filtered relevant lines and
the staged manifest; unrelated desktop-window observations are not retained.
The observer deliberately times out its owned process, so this test does not
assert normal exit or editing; the separate Write lifecycle test covers close
and restart, not editing.

The current selected WOW bridge was rebuilt into a controlled provider test.
Its one-byte BOOLEAN return rejected false attach, preserved thread and detach
returns and invoked both cleanup hooks; the Windows loader exercised eight
attach/unload cycles. `dll-entry/` retains outputs and input hashes. The
provider is controlled test code, so this is not an actual guest DLL attach
failure or a proof of all production loader interactions.
