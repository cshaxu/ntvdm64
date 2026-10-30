# Pending USER, TOOLHELP and REGEDIT guest C source roles

The mixed inventory contains 12 USER, ten TOOLHELP and 12 REGEDIT pending C
paths. The [per-file census](user-toolhelp-regedit-source-census.json) checks
each against its OpenNT original: all 34 are byte-identical and have
same-stem objects in their respective original makefiles. Those makefiles
use 16-bit compiler and linker rules for the guest USER executable,
TOOLHELP DLL and REGEDIT executables. Their hashes are recorded; no guest
build was performed and makefile conditions have not been fully evaluated.

None of these exact paths appears in the selected x86 worker or WOW32 Ninja
commands or recorded dependencies. Query positive controls are recorded in
the DOS crosswalk. The [bounded dispositions](user-toolhelp-regedit-host-dispositions.csv)
exclude these guest source roles from the inspected host-input denominator.
This does not exclude USER, Toolhelp or registry functionality: the modern
host adapters and guest-to-host identity, message, callback and lifetime
contracts remain subject to the relevant S11–S19 runtime acceptance. Other
configurations and guest behavior are not proved; the frozen mixed-scope
ledger, guest images and mirror files are unchanged.
