# S11 retirement error candidate integration

The current ten-input candidate was staged after rebuilding WOW32 with
ADAPTER-WOW-108. Its immutable guest manifest verified 59 files (configuration
excluded). The isolated Console-17 and Window-17 exit-status matrices passed;
the exact product manifest and machine-readable summaries are alongside this
record. The selected-object graph fixture and synthetic two-failure evidence
are recorded in the baseline fail-fast trace review. These results do not
identify the observed SOL 1400 cause or substitute for the full product gate.

The Control suite also completed successfully against the same ten-image
runtime, with runtime-manifest and gate timings retained here. It covers RPC,
GUI launch, console handoff, relaunch and worker/frontend loss paths; it is
not the Full suite and does not resolve the separate Winmine test restriction.

The candidate also passed the same-worker idle/dialog witness (two tasks,
17 dialogs, 16 explicit closes, surviving task idle and both retirements).
Separately, visible-desktop probes passed Notepad Find, Write Find and Program
Manager New Group dialog close, parent recovery and launcher exit. Individual
probe outputs and manifests are retained here. These are finite functional
checks, not performance acceptance or proof against the intermittent 1400.

The rebuilt candidate also passed the original Win16 Calculator startup,
About dialog, parent recovery and launcher-exit witness; its probe result,
manifest and source-file hashes are retained under `calc/`. The static
WIN87EM import is not proof of the loaded module identity, numerical FPU
behavior, or the pre-Windows-3.0 special loader branch.
The four visible-dialog probe manifests were compared against all ten sealed
candidate image hashes and matched. The idle/dialog test uses a fresh package
identity; all ten underlying input hashes matched the candidate.

The current selected WOW32 bridge was retested with both a direct fixture and
eight real Windows loader cycles using a controlled provider. Both passed;
the current link map identifies DllMain in wow32-dll-entry-bridge.obj and
W32DllInitialize in wow32.obj. Test and selected-image hashes are retained
under `bridge/`. These controlled-provider results do not exercise an actual
WOW guest DLL failure or unload lifecycle.

The current selected-object `native-nested` fixture passed against the rebuilt
provider, including nested ownership, native wait unwind, exceptions,
window destruction and dead-target error paths. Its logs and hashes are in
`native-nested/`. These controlled cases do not identify the intermittent SOL
1400 or prove all real callback interleavings.

The current candidate also passed the actual-worker VDD/MEM export witness:
query, allocation, reallocation and free, plus exhausted reallocation preserving
the allocation and free space. The unmodified guest MEM and launcher completed.
This run uses a fresh package identity; all ten underlying input hashes match
this candidate. Evidence is in `export-live/`. This is not exhaustive consumer,
concurrency or host allocation failure coverage.

The current candidate's `ntvdm.exe` also passed the direct ordinal ledger
check: 271 contract entries and 271 named exports. This checks names and
ordinals, not the behavior of every exported function.

After the recorded tests, a Ninja dry run of the eight x86/x64 component
build graphs' default targets reported no work. All ten candidate input hashes
matched their corresponding current cached images; `current-cache-hashes.json`
records the paths and hashes. This freshness check is not a fresh full build,
Full product test, or a publication.

The selected CCPU far-destination fixture was rerun against the current worker
build. Trace disabled, matching selector and nonmatching selector all retained
#GP 13/error 8 for the invalid descriptor; the valid control returned normally.
Result and symbol-owner hashes are in `far-exception/`. This tests a controlled
selected-object fault boundary; it does not prove complete guest exception
handling or the separately noted opt-in CALLF trace behavior.

A further selected-object graph fixture explicitly verified forward cleanup:
a pending low-index entry fails with ERROR_NOT_FOUND, a later pending entry is
actually reclaimed in the same sweep, the first error survives that success,
and the first entry is reclaimed after its synthetic identity is restored.
The full fixture passed; logs and input hashes are in `continue-after-error/`.
This is a controlled failure, not proof of the intermittent SOL cause.

A later source-only correction clarified that idle intent is worker-shared.
Rebuilding WOW32 after that comment produced a different full-file hash from
this tested package. A PE section comparison shows equal `.text`, `.data`,
`.rsrc` and `.reloc` hashes, but differing `.rdata`; see
`wow32-comment-rebuild.json`. The comparison does not establish binary or
runtime equivalence. Consequently this package remains tested evidence but
is no longer the latest cached build; restage and validate the eventual release
candidate rather than attributing these receipts to the rebuilt DLL.
