# S13: remove ten unselected mirror files

The three audit records r610–r612 identify the files, their original source
hashes and the product and test references examined. Two static-data source
files were only compiled by a historical test generator and were removed with
that generator and its obsolete verifier. Product executables, guest binaries,
fonts and tools that remain independently useful were not removed.

The cached x86, x86-extra and x64 product builds completed successfully. Of the
ten staged images, eight match the previous published images byte for byte;
NTVDM and nthook32 differ only in the four-byte PE timestamp. The earlier
r481 Full 17/17 product evidence is therefore reused under the owner's rule;
this is not a fresh Full run. The modified negative-guard test was run using
the real x86 MSVC preprocessor and passed 184 retained guard witnesses plus
its additional checks. Its initial failures and reconciliation are described
in [r614](s13-r614-ledger-reconciliation.json).

All ten staged images were published and hash-checked in both `assets/release`
and `O:/winnt/system32`. This batch is a progress delivery, not completion of
the full mirror audit; the supported-context inventory remains incomplete.
