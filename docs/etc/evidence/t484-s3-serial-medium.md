# T484 S3 P150 serial medium fixture producer

## Disposition

B0237 remains useful: nt_com_mock_fixture.c includes the actual current host
serial implementation and substitutes the Win32 device medium. It verifies
successful open, one-byte write, receive/read and close with lifecycle counts.
The existing baud-arm and absent-device discovery tests do not replace this
normal-medium contract. This is a unit fixture, not real serial-hardware or
Full acceptance.

Compilation moves from PowerShell-generated CL commands to the existing
CMD/native fixture graph, target serial-medium (x86 CCPU40 /MT). Preserve its
original definitions, forced nt.h, include cohort and link libraries. The
runner retains execution and success assertions only. No original guest or
product implementation changed; no separate producer/cache is introduced.

## Verification

Owned background PID9556 built the target and passed the original
T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK assertion. The original /W4 profile
still reports macro/type and legacy-source warnings; this is not a warning-free
build claim. Fixture source and behavior were not changed to silence them.
PID21120 ran cached full x86/x64 production build and verified11 raw images
and22 dual-publication hashes unchanged. No new publication is necessary or
claimed. These are focused results, not a newly executed Full suite.

[Receipts](../../../artifacts/build-migration/t484/s3-serial-medium/result.json).
B0237 complete;266 of355 dispositions complete,89 remain. S3 continues with
B0238; earlier unresolved cases and final clean acceptance remain open.
