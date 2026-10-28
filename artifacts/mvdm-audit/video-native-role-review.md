# Original video entry and CPU-interface reconciliation

This S10 checkpoint follows `1b506b719`. All seven remaining pending
`base/video` C files are reviewed: five selected integrations become partial,
one alternative CPU-interface physical file is excluded, and EGA_DUMP tracing
remains pending. No production source, original guest or runtime image changes.

## Source, audit catalogs and actual image

The [observer](video-image-observer.py) and [observations](video-image-observations.json)
bind seven DOS16 catalog rows and fourteen OpenNT/OpenNT-4.5 WOW32 IDs.
Both editions' original files are freshly hashed against the frozen union;
every current cohort file exactly matches primary OpenNT. Catalog Git-blob
fields are identity metadata, not newly reverified index claims. Catalogs,
scope, original NT recipe and object/map/image inputs have exact hashes.

The byte-identical original NT video recipe selects ega_dmmy, ega_mode, gvi,
vga_vide, video_io and ega_trcr. The actual Ninja graph/archive and VALID
dependencies preserve these selections. The five active cohort objects have
**21 mapped functions**, matched by non-relocated code bytes at actual
candidate addresses. Candidate executable sections equal the cached
map-producing worker. Call relocation bytes are not all checked by this test.

All **29 generic-video, 29 EGA-video, three CPU40-read and six CPU40-write
initializer pointers** are separately resolved from actual COFF relocations
to their linked-image targets. Original BOP10 resolves to video_io; BOP42
resolves to ega_video_io. These are initial-image facts, not proof that later
host/adapter table mutation has correct ownership or lifetime.

## Per-file disposition and retained capability

| File | Evidence-backed current role | Remaining boundary |
| --- | --- | --- |
| ega_dmmy.c | Partial. Original get_ega_switch_setting returns zero, and ega_ipstat0_inb places that value in switch_sense. This is an original fixed-switch provider, not a newly invented project success stub. | Actual port observation and adapter initialization are not tested here. No EGA hardware/presentation completeness is accepted. |
| ega_mode.c | Partial. Original initialization installs DUMMY_FUN; choose_ega_display_mode recalculates pitch/wrap/split and munged mode, selects update/paint callbacks and requests refresh. | Every mode/index, transient CGA geometry, fonts, split/wrap, reset and concurrent paint-provider retirement need S13 proof. |
| gvi.c | Partial. Original adapter choice/fallback and soft-reset old-adapter termination precede initialization; recalc_screen_params preserves original Video7 text pitch rules. | Adapter transitions, byte/word/dword geometry, clears, repeated resets, host callback failures and retirement remain S13. |
| vga_vide.c | Partial. Fourteen original VGA palette/DAC, pixel/character/scroll, display-information and selective-state functions are image-bound. | Invalid requests do not reach these functions. Valid memory/format/buffer-size, font/palette/pixel, save/restore, failures and lifecycle remain unproved. |
| video_io.c | Partial. Original range checking, stream-I/O transition and generic function-table selection are retained. Real guest invalid requests prove one bounded return branch. | Valid entry dispatch, actual presentation, stream ownership and failure/retirement still require proof. |
| ga_glue.c | Excluded alternative physical CPU-interface file. The original NT recipe explicitly comments it out; its a2cpu/2.0 glue is displaced at the selected CPU40/C_VID interface, not by a modern rendering replacement. | Read/write/fill/move capabilities remain at the selected original EVID owners, which remain partial. Direction, address units, RAM/video identity, faults and retirement are not excluded or newly accepted. |
| ega_trcr.c | Pending diagnostic profile. Original EGA_DUMP wraps file-backed EGA access tracing; current object has no mapped function. | Resolve the T-wide diagnostic profile, trace-file setup/failure/close and hook ownership before any exclusion or implementation. Map absence is insufficient. |

For ga_glue.c, this is more than current nonselection: the original NT recipe
comments out that exact file; actual CPU40/C_VID flags select the original
EVID branch in ega_read/ega_writ; all nine Glue_reads/Glue_writes initializer
targets resolve to `original-softpc-cvidc:ev_glue.obj`; the linked map has no
legacy glue_b/glue_w/glue_str owner. The selected EVID provider retains its
existing registered mirror differences and partial obligations. No capability
is declared unnecessary merely because the legacy interface is absent.

The [six-row disposition ledger](video-native-role-dispositions.csv) records
source identity, actual chains, ratio basis and finite S11/S13 obligations.
Source text retention is 100%. Whole restored-code percentages remain
explicitly unmeasurable from these linked functions and bounded invalid-entry
cases; they are not inferred from function counts. No mirror diff/exception
is introduced. Existing different mirror files remain partial.

## Real guest return-contract witness

The independent [BIOS dispatch probe](../../tests/observation/bios_dispatch_probe.asm)
adds a compile-time VideoInvalid workload, preserving the earlier normal
BIOS and dummy modes. Each accepted/candidate positive run performs:

- **908 generic BOP10 calls**: AH 1D..FF, four passes. These fall outside the
  original selected VGG EGA_FUNC_SIZE=1D and preserve the asserted state.
- **904 BOP42 EGA calls** over the same range, excluding valid Video7 AH6F.
  The original handler rejects them and sets Carry; other asserted flags
  remain unchanged. No valid Video7 capability is rejected by the probe.
- Each call checks AX/EAX upper word, the tested 32-bit general registers,
  SP, DS/ES and defined arithmetic/direction flags. Index progression retains
  all four flag patterns for each tested AH, including the skipped AH6F.
- Both packages pass **1,812 cases**. The deliberately wrong AX expectation
  is rejected at phase V with guest exit 1 on each package; no success marker
  is allowed in either negative result.

The source trace distinguishes the two intentional semantics: video_io ignores
out-of-range requests; ega_video_io explicitly calls setCF(1). The probe tests
that difference, rather than flattening it into one modern-host behavior.
It changes no original guest, medium, handler or device configuration.

Two further accepted-package default BIOS controls pass: the original 836
cases and its wrong-oracle failure at phase C. Its ordinary COM SHA-256 is
still `F6E667ABFE8FE83EE9CA31B31C66D7460D662B644C4153724000172D17CAFEE3`.
Thus the added compile-time mode does not weaken or replace that workload.

All six runs are serial, private-desktop, medium-integrity modern-pwsh runs
through the existing [guest runner](../../tests/observation/verify-ccpu-sign-flags-guest.ps1),
short physical stage and package-scoped RPC. Exact commands, parent token,
PID/start/exit, tool/source and runtime identities are captured. No Computer
Use or UI automation is used. The first supervisor reused a historical final
banner mentioning disk reset; its actual per-case arguments, strict runner
assertions and screen results select VideoInvalid. That banner is retained
verbatim and is not acceptance evidence.

## Delivery and remaining work

Counts become **1,555 pending, 441 partial, 53 full and 2,635 excluded**.
The exact-delta checker verifies all six changes, prior full-index/source and
frozen identity, unrelated ledger/shard preservation, 21 functions, 67 pointer
initializers and all six guest/oracle outcomes. Original valid-mode and
rendering/lifecycle obligations remain in the [worklist](recovery-worklist.md).
This does not qualify all DOS/WOW/Win3.x video, explain the held candidate's
SOL failure, authorize publication or close S10/T473.

The [delivery result](video-delivery/stdout.txt) passes the exact six-row
delta, source/image and [225 captured metadata hashes](video-runs/manifest.json),
all six actual guest results and input identities, unchanged accepted and
candidate packages, documentation/link and diff checks. The
[delivery verifier](video-native-delivery-verification.ps1) is the current
reproduction entrypoint; older fixed-count checkpoints belong to their named
checkouts. Generated COM/EXE media remains in disposable build storage.
