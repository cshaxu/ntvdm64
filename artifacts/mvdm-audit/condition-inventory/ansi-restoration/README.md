# ANSI mirror restoration

Owner-approved restoration of the four original files changed by
MVDM-HOST-DIV-479. Their content matches commit `08755b007^` byte for byte.
The former ANSI/pre-ANSI selections and original `stubs.c` undefinition are
restored; authored build commands do not acquire SoftPC profile flags.

The cached x86/x64 product build passed; the 428-command profile check passed.
Same-package-identity comparison against the prior release found timestamp-only
changes in nine images and 558843 changed bytes in NTVDM (including .text,
.rdata, .data and .reloc). This comparison is not a proof of semantic
equivalence; the change was treated as a product change and the actual staged
ten-image candidate passed all 16 Full gates. The Win3.1 Standard/386 by
DOSONLY 0/1 matrix passed with normal exit; 68 guest binary hashes in both
the original installation and isolated copy remained unchanged. No original guest binary was modified or rebuilt; the matrix assembles its
own test-only probes. The tested ten-image package was installed and
hash-verified at both release destinations. See the adjacent machine-readable
results and `assets/release/release-manifest.json`. Physical manual input and
exhaustive applications are not covered by these automated observations.
