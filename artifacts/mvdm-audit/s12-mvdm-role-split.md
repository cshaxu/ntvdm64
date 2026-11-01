# T473 S12 MVDM source-role split

Owner direction places guest-executed source in `src/mvdm-guest/`, including
`dos/v86`, `wow16`, `bin86`, `dpmi`, `sim16`, `v86/util`,
`softpc.new/bios`, `softpc.new/roms` and `softpc.new/data`. Four guest-side VDD example subtrees
also move: `vdd/samples/hpscan`, `mscdex/tsr`, `sample1/fax16`, and
`sample2/16bits`. Other original MVDM paths move to `src/mvdm-host/`.
Both retain their original relative paths and source provenance. The move
does not rebuild or change guest binaries. Host device simulation, `dpmi32`,
`wow32`, CCPU, host BIOS services and host-side VDD examples remain in the host
root. Actual execution role, not filename or service relationship, determines
the split; software V86 remains distinct from the retired hardware monitor.

The [path and Git-blob manifest](s12-mvdm-role-split.tsv) records 2,704 guest
and 1,914 remaining files relative to baseline `0246d024b`. All 4,618 working
source objects match their baseline blob identities. Before
staging, 2,958 text-only
working files had checkout line-ending differences; they were restored to the
existing Git blobs to prevent the move from committing spurious changes.
No executable, guest image, ROM, or other binary was changed or rebuilt.
The old mirror's no-text-conversion Git attribute is applied to both new roots.

The SIM16, DOSX and guest VDD sample makefiles use 16-bit linkers; `quit.asm`
declares V86 guest execution. This source and the firmware were moved as
original bytes, without building guest artifacts. Header and shared declaration
roles do not alone imply guest execution. Further mixed or unclassified source
remains subject to actual-entry audit, not a claim of universal classification.

Build generators, direct source includes, guest staging and archive tools,
selected audit tools and current authority references are updated. Historical
source records remain historical. Cached x86/x64 product builds and the
428-command profile audit pass; documentation governance passes. With the
existing package identity, eight images differ from the prior release only by
PE timestamps and one is identical, while NTVDM differed in 44,218 bytes. That
first candidate passed the selected Full suite with DOS4GW/NESticle skipped for
lack of the external fixture. The expanded candidate also passed that suite.
After the final firmware-data move, cached x86/x64 incremental builds required
no compilation. All ten raw build inputs exactly match the tested package's
input SHA-256; after applying the same package identity, all ten images are
byte-identical to the tested package, not merely timestamp-equivalent. No Full
rerun was needed. The tested package was published to both release destinations;
[hashes](s12-mvdm-role-split/raw-and-published-hashes.json),
[Full timings](s12-mvdm-role-split/full-timings.json), and
[package comparison](s12-mvdm-role-split/package-comparison.json) are retained.
Physical input and the skipped external DOS4GW fixture are not claimed as passes.
