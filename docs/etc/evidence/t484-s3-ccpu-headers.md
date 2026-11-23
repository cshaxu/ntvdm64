# T484 S3 P80 selected CCPU preprocessing producer

B0108 no longer parses old /Fo object commands or emits compiler scripts in
PowerShell. A native worker target obtains each selected owner's current source
and COMPILE_FLAGS from CMake, asserts CPU40/SPC386, and preprocesses exactly
c_div64, c_tlb, c_tsksw, c_xfer, c_xtrn, c_main, c_reg and zfrsrvd. The explicit
observation target always reruns so changes in included headers cannot leave
stale observations. It writes only diagnostics/ccpu-headers, never production
objects, libraries or images. It is not an ALL/product dependency.

The runner keeps the BuildRoot/WorkerBuildRoot interface, bounded process
ownership, before/after production hashes and native command/dependency logs.
The obsolete separate CompilerDriver override is removed; no current caller
uses it. The CMD entry selects the existing worker toolchain. Dependency
inspection now uses actual CMake object names rather than copied obj aliases.
The first run preprocessed successfully but Ninja rejected absolute object
names. The corrected runner uses normalized graph-relative names and requires
eight VALID dependency records, rather than accepting a successful empty log.

[Observation result](../../../artifacts/build-migration/t484/s3-ccpu-headers/observation-result.json)
records the eight selected profiles, output hashes/sizes and unchanged cached
production inputs. Preprocessing does not type-check or demonstrate runtime
behavior; those are explicitly outside this observation's claim. No mirror,
guest or production algorithm is changed. Raw attempts and final verification
are under build/t484-s3-ccpu-headers.

Cached full build PID32216 and final verification PID13024 exit0. CMake
combines the long commands in a native batch file; the first final assertion
expected inline commands and failed. The corrected observer records that
batch's hash and actual text, then verifies eight native /P commands and
eight VALID dependency records. It does not hide the batch behind a pass.
x64 selection is rejected. All11 raw host inputs and22 published hashes
remain unchanged, verifying both existing publication destinations.
B0108 complete: fixed355 inventory156 complete,199 remaining (52 pending
producer/staging/verification items,147 awaiting reconciliation).
