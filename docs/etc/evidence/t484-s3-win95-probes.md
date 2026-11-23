# T484 S3 P70 Win95 independent probe producers

B0096/B0097 now share CMD/native win95-probes production for loader, menu and
DOS restart diagnostics. Existing CL flags, CRT-free link, libraries, entry,
x86 ABI and old output names remain. One narrow assertion runner copies the
selected native image/map and verifies the PE version fields; it no longer
writes compiler scripts, invokes cl/link or edits binary bytes. The loader and
Menu/DosRestart entry arguments remain compatible.

The native target uses installed EDITBIN /subsystem:windows,4.0 after LINK's
existing /osversion:4.0. An isolated experiment established that EDITBIN emits
LNK4241 yet actually writes the requested subsystem version. Each final image
is checked for OS/subsystem4.0 and x86. No new generator/helper is introduced.
Only independently authored test images are processed, never original guest.

Actual old/new builds produce equal sizes: loader5632,menu7168,DosRestart8192.
Initial strict comparison failed on an additional byte in each image; mapping
the PE debug directory identified those offsets4504,5380,6584 as debug timestamps.
Each equals its own COFF timestamp. Final comparison permits only COFF/debug
timestamps and the PE checksum, independently recomputed and validated. No
other byte differs. EDITBIN generates a valid checksum where the prior direct
byte edit left the previous value. The initial failed receipt remains evidence.

Raw process/log evidence: build/t484-s3-win95-probes. Concise receipts:
artifacts/build-migration/t484/s3-win95-probes. These are producer/PE validations,
not fresh Win95 runtime acceptance; no diagnostics are launched inside guest.

Cached full host build passed. Three native probe hashes and timestamps remain
stable on no-op rebuild; x64 target is rejected with exit2. The11 host input
and22 publication hashes remain unchanged, so no recopy is needed. Governance
passes. Fixed355 inventory:147 completed,208 remaining (51 pending producer
chains and157 awaiting disposition reconciliation).
