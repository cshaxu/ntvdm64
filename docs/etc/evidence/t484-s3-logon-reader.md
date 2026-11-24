# T484 S3 P161 shared native environment reader

B0028/B0029/B0030 still accepted an external C# LogonHelper and loaded it through
Add-Type, despite their DOS probe producers already being CMD/native. Reuse the
existing native logon-environment.exe and extract the already delivered latency
runner's read/timeout/disposal logic into test/observation/logon-environment.ps1.
All four callers now share that same reader. Values stay in memory; process
records contain only PID/start/image and existing reports contain names/lengths.
Remove the three obsolete LogonHelper parameters; no current repository caller
passes them. Do not change guest instructions, assertions or original media.

The existing ccpu_alu_oracle.c had no current native target. Add the x86-only
ccpu-alu-oracle fixture to the existing CMD entry. Its native x86 instructions
produce independent reference data; it is not a replacement guest implementation.
Compilation retains one existing fopen deprecation warning (C4996), not a failure.

Evidence: build/t484-s3-logon-reader. PID22772 built the native reader, checked
all five script syntaxes and unfiltered environment entry shape, then ran actual
SignFlags and Transfer guests successfully. PID14092 built the ALU oracle and
passed actual ALU execution:48 phases,1086848 records,3260544 instructions.
PID12192 owns the three negative-oracle runs, cached full build and11/22 package
hash verification; final-result.json records its final result. Guest runs use
the normal medium-token private-desktop observer and owned short physical stages.

Latency's inline reader was moved verbatim; this batch does not claim a new
WINMINE latency/gameplay result or close B0063. Full and S4/S5 remain outstanding.

PID12192 passed all three negative cases and cached full/11+22 identity checks.
No host image changed, so no republication was required. Final governance
PID23872 passed.293/355 paths disposed;62 remain. Temporary package and generated
ALU reference-data copies were removed after their hashes/results were retained.
