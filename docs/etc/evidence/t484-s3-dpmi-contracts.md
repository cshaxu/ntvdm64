# T484 S3 P26: native DPMI and port contract producers

Three runners now delegate binary production to CMD/native targets. Port
privilege reuses the current CCPU bit-fault fixture's actual profile and link
closure, substituting its one fixture object. The existing /force:multiple
policy is a bounded test seam, not a production link claim. DPMI SWINT compiles
the actual dpmiint.c with its current profile plus test-only function sections.
PM entry uses the same current complete function projection as its old runner;
normalized LF bytes are identical. No mirror or original guest is modified.

The SWINT test needed the original TIB provider's printer callback dependency.
A same-ABI NtVdmControl test provider aborts if reached; no printer behavior is
invented or accepted. The first PM invocation rejected its evidence directory
because the old runner compared mixed path separators. Canonical path checking
fixed that test setup, and the formerly unstarted case passed separately.

Final evidence:

- Port privilege:117 scalar/string width, privilege and memory cases, actual
  CCPU/IOS map owners checked; each process has a60-second limit.
- SWINT: selected handler ownership,16/32-bit stack widths and bounds, mapping
  failures, nested fault/interrupt returns, fault rollback and vector14 frames.
- PM entry:65536 FLAGS combinations plus two descriptor-boundary cases.

The [port receipt](../../../artifacts/build-migration/t484/s3-dpmi-contracts/port-result.json)
retains its bounded device/TSS and duplicate-symbol limitations.
The [initial runtime receipt](../../../artifacts/build-migration/t484/s3-dpmi-contracts/runtime-result.json)
records the failed PM setup honestly; its
[final receipt](../../../artifacts/build-migration/t484/s3-dpmi-contracts/entry-final-result.json)
is the successful replacement. These are selected native contracts, not guest
DPMI, DOS4GW or full runtime acceptance.

Cached full host build and separate11 raw/22 publication identity checks passed;
unchanged product images need no recopy. The containing commit delivers P26.
S3 remaining producers and the S4/S5 obligations remain active work.
