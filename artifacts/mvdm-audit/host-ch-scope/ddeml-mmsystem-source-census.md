# Pending DDEML and MMSYSTEM C source roles

The mixed inventory has 13 pending DDEML and 17 pending MMSYSTEM C paths.
The [per-file census](ddeml-mmsystem-source-census.json) verifies all 30
against the OpenNT originals: every pair is byte-identical. Each same-stem
object is named in its original makefile. DDEML's `OBJ1`/`OBJ2` rules compile
the corresponding C sources with the original `cl16` and link them into the
16-bit DDEML DLL. MMSYSTEM's object lists, `.c` inference rule and `link16`
produce its 16-bit DLL. Makefile hashes are recorded in the census;
conditional build branches and guest binaries have not been rebuilt.

None of these exact paths appears in the selected x86 worker or WOW32 Ninja
commands or recorded dependencies. The graph queries' positive controls are
recorded in the DOS pending crosswalk. The [per-file host dispositions](ddeml-mmsystem-host-dispositions.csv)
exclude these historical guest source roles from the inspected host-source
denominator. This is not an exclusion of DDE or multimedia capabilities:
thunks, host services, callbacks, task lifetimes and real guest behavior
remain stage-specific runtime obligations. Other build configurations are not
proved by this census, and the frozen mixed-scope ledger is unchanged.
