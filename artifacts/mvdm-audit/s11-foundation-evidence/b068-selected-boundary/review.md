# B067/B068 selected BIOS fallback and provider boundary

The source audits' B067-R03 and B068-R01 through R04 remain open: they
distinguish ordinary WOW hosting, the CCPU/SAS fallback, and enhanced-mode
Insignia driver/NIDDB instances. The external audit CSV hashes are in
`inputs.csv` and `original-inputs.csv`. None of those rows establishes that
ordinary WOW must start or initialize the external driver.

The latest published NTVDM manifest identifies cached input SHA-256
`86835839AC83A4FF61493C049264FAD15B3255AAC74C11D3E0E2F75A8B547E4B`.
Its map identifies `biosDoInst` and `c_SasRegisterVirtualSelectors` in the
selected original CCPU object, and `virtual_device_trap` and `BIOS` in the
selected original BIOS objects. `verify-bop-slot.py` reads that exact PE and
map and verifies the actual BIOS table's `B1` slot points to
`virtual_device_trap`. This is binary selection, not an observed BOP call.

A separate read-only Windows resource probe compared all 28,420 bytes of
`SOFTPC_BIOS1` in the published `assets/release/ntvdm.exe` to the selected
repository ROM and found them identical. At offsets 30B0, 30C0 and 30D0 the
bytes encode byte, word and dword reads respectively, matching `bios1.asm`.
The legacy-signature bytes at 3030 are `ED C4`, not `66 90`; this identifies
the embedded bytes, not the result of a running BIOS fallback. No ROM was
rebuilt or modified.

Source tracing shows the conditional B1 registration path assigns the two
guest-provided selectors and writes descriptors. The no-paging and successful
translation paths do not use this fallback; protected-mode fallback checks the
registration flag, whereas the other branch uses the ROM segment. The
original selected word and dword fallback return expressions cast to `IU8`;
the same four expressions exist in the pinned original source. This is a
source-proven width mismatch conditional on that route, not a reproduced
guest failure, and the owner's boundary-only instruction excludes an
independent CCPU-internal fix. NIDDB cleanup in `virtual.c` does not itself
establish selector invalidation; actual VM/LDT ownership remains unresolved.

The explicit `INSIGNIA.386`/`SPCMSWD` guest provider was not located in the
previously indexed original trees or available loose files. The two locally
available NT 3.5 DDK and NT 3.51 CD directory listings also had no matching
filename; raw read-only string searches of the separate SoftPC Win3.1 images
did not find either name. These are bounded negative searches, not proof of
global absence or analysis of nested/compressed archives. The currently
installed Win3.1 configuration has no explicit Insignia driver entry; that
alone cannot rule out indirect loading. No disk image was mounted or changed.

Per-file source roles remain `partial`: `ccpusas4.c` has an estimated 99.91%
matching original code text, `virtual.c` 100.00% despite a byte-level diff,
and byte-identical `bios.c` remains partial because its full behavior has not
been accepted. The percentages are not execution coverage. S11 owns the
selected execution/identity boundary and cannot claim the external caller was
verified; S13 owns actual device/selector registration and reset evidence,
and S14 owns any demonstrated Win16 callback implications. A supported real
provider, ordering and width witness must be established before any repair or
capability exclusion. No source, product, guest or system configuration was
changed by this review.
