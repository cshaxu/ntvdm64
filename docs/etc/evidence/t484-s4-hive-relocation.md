# T484 S4 P34: layout-preserving Win95 hive preparation

## Question and decision

Can the existing native PATH95 replace the remaining Python hive producer
without changing isolated guest configuration? Real SYSTEM.DAT and USER.DAT
comparisons found equal substitution counts (143 and131) but ordinary PATH95
rebuilds allocation layout:332872 to352772 bytes and45088 to49184 bytes.
It is not a drop-in replacement for the previous length-preserving producer.

The admitted configuration migration therefore extends the existing native
parser with explicit replace-equal. It requires CREG and nonempty equal-length
roots; preserves original entries, padding and block/free metadata; updates
only selected REG_SZ bytes and enabled RGDB checksums. No duplicate parser,
new executable, guest binary modification or default replacement change.
The readonly original installation remains untouched.

## Implementation

- tools/relocate-guest-hives.cmd checks the copied directory owner, prepares
  both fresh candidates before installing either, and propagates failures.
- New-Win95ProbeCopy calls that CMD owner. Existing failure cleanup remains.
- The replaced relocate-win95-hive-equal.py is removed, not kept as a fallback.
- test/addon/path95/verify-relocation.ps1 exercises the released tool with
  length/roundtrip/source checks and no-op/existing-output/unequal/truncated
  negatives. Case-insensitive replacement writes requested root casing, so
  reverse comparison uses a canonical-casing input; raw reverse is not a
  valid assertion for mixed-case source paths.
- README build/publication instructions now name current CMD entry points.

## Evidence

Root: build/t484-s4-hive-relocation. Initial output comparison and the previous
producer are retained there as diagnostic evidence, not current build inputs.
Both native replace-equal outputs match the old producer exactly:

- SYSTEM.DAT:0E89E0E53A8632E4A9022B81554B2B9753815AE20D79668FFB53B84713B6CC01
- USER.DAT:2CE9348997DE3F9C9D5DD9909CE1C4310308417E14C078CCC6FCFB66F3C0B847

Cached full build and addon publication PID29820 completed exit0,
21:13:32Z–21:14:44Z. All22 host publication hashes match; PATH95 and its addon
manifest are the changed release products. No mirror diff or host behavior
change. The first assertion run failed querying hidden SYSTEM.DAT without
Force; the next exposed the incorrect raw reverse assumption (USER has8 case
bytes and4 checksum bytes). Both failures are retained. They did not justify
changing the relocation algorithm.

Final PID29356 completed21:16:56Z–21:17:03Z exit0, checking1166 source files.
The failed-pair test returns1 after creating the first candidate but leaves
the first installed hive unchanged; its owned directory is cleaned.
Final published-tool and actual Win95 preparer results are recorded in
`test-verified-result.json`; source-hash checks and owned-copy cleanup are part
of that run. This is preparation equivalence, not a new interactive Win95 or
Full acceptance claim.

## Remaining scope

This removes the hive producer dependency, not the whole Win95 caller.
INI/driver/profile preparation still requires migration. The frozen census
remains348/355 disposed,7 pending, with five overlapping supplemental chains.
S4 and S5 remain open.
