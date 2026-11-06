# T478 S23 — original palette returned-count conversion

Baseline: 6e8ad9dff. Finding 55 is repaired with one converter-count change,
MVDM-HOST-DIV-878. Before this change, the complete wgpal.c token stream matches
both pinned original families after comments/#line are removed. This is an
original bug correction, not a replacement GDI or palette implementation.

## Behavior and correction

For a two-entry palette, requesting four entries starting at index one returns
one native entry. Original WG32GetPaletteEntries allocates four and converts
all four, including the temporary allocation tail untouched by GDI. It also
converts unavailable entries when the native return is zero. Pass ul, the
unchanged native return, to the existing PUTPALETTEENTRY16 converter instead.
Allocation, native handle conversion, result and release remain unchanged.
No new validation, retry, zero fill or driver behavior is introduced.

WG32GetSystemPaletteEntries already converts ul, including its original
256-entry RGB fallback. It is unchanged. The converter writes directly to
mapped guest memory and current CPU40 flush is a no-op; no mapping/flush
implementation or unsupported selector contract is added. Query-only/null
pointer and zero-sized allocation behavior are not broadened. A final native
zero-capacity observation returns zero with a non-null output for both palette
and kerning APIs; null output instead returns the available count (2/120 in
these fixtures). The extra probe source and output are retained in the receipt;
this is native boundary evidence, not another guest test.

## Verification

[Native fixture](../../../test/integration/wow_palette_output_test.c) and
[runner](../../../test/integration/verify-wow-palette-output.ps1) reuse the
existing exact-body extractor for both original thunk and converter. Native
GDI and palette objects are real; guest mapping and allocation contents are
explicit test seams. Tests cover short, exact, past-end zero and null-handle
zero results, converted byte counts, values, tail preservation and retirement.
Baseline: 19 checks, five failures. Fixed: 19 checks, zero failures. Initial
fixture compilation required a scoped warning suppression for the unchanged
original assignment-in-condition and unsigned fixture constants; these were
test build issues, not additional product defects.

The [independent Win16 witness](../../../test/integration/wow_palette_guest.asm)
and [isolated runner](../../../test/integration/verify-wow-palette-guest.ps1)
create the same palette via real GDI16 ordinal 360 and query ordinal 363.
The prior S22 package returns one but fails the three-entry tail sentinel.
The current package passes and exits zero. The witness also checks exact two-entry
RGB results, a valid past-end zero result, unchanged tails and DeleteObject
success before ordinary exit. No guest mirror/media is modified. The isolated
runtime helper gives each copy a fresh package identity; its WOW32 hash is
recorded separately from the source package manifest hash.

[Receipt](../../../artifacts/mvdm-audit/shard-3/palette-output-s23-results.json)
retains comparison, native/guest and release evidence. These bounded tests do
not establish arbitrary display-driver, task-switch, palette-message timing,
invalid selector or device color fidelity guarantees.

## Full first-attempt failure

The first Full attempt stops at Console-17 command-c with 1817
(RPC_S_NO_INTERFACES), before a worker exists. Its observer records a live
NTSRV child and unavailable worker image identity. RUN16 and NTSRV input
SHA-256 values exactly match the preceding S22 package, which passed Full.
The service exposes its endpoint before registering its interface; management
interface discovery can return that status, and RUN16's existing bootstrap
loop retries unavailable/access-denied but returns 1817. This is a source-
supported startup-window inference, not a traced proof of the exact schedule.

The failed attempt remains a failure in the receipt. It is not attributed to
the palette correction, suppressed, or repaired with a new retry policy in
this mirror packet. A full replay uses the same frozen package and separate
logs. Repeated startup failures require a launcher/service investigation;
this S does not claim that startup risk fixed.

## Delivery

Coherent x86/x64 build passed. The unchanged-package Full replay passes all
16 gates in 550557 ms; the first failed attempt remains separately recorded.
The ten released images and manifest exactly match frozen package
fafb7ffe-9237-44a1-84bd-8e8f83ba4a05 (0.0.478, protocol 67).
DOS4GW/NESticle remains owner-excluded, skipped rather than passed.
Only this worktree assets/release was published; no O:/winnt write or push.
S24/finding 56 follows after S23 delivery.
