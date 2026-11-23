# T484 S3 P47: native message witness composition

B0045 previously assembled SEND/RECV and compiled MSGHOST directly in the
PowerShell runtime runner. A native CMake target now owns those commands,
their source/header dependencies and the original feature definitions. CMD
accepts a bounded numeric feature mask; the runner retains its existing
semantic combination checks. The external-native-host path builds only the
guest witnesses, preserving the supplied peer/host identities.

The thirteen exercised masks are 0,1,6,14,30,38,70,134,262,518,1542,2566,4358.
They cover each existing feature, including sender-only transaction injection,
receiver selection, DDE task loss and synchronous DDE diagnostics. Each of the
26 guest executables and26 listings matches the old assembler command bytewise.
All13 native DLLs match the old CL command except actual PE timestamp fields;
raw hashes are retained separately, not reported as identical.

[Witness verification](../../../artifacts/build-migration/t484/s3-message/verification.json),
[PE comparison](../../../artifacts/build-migration/t484/s3-message/comparison.json)
and [contracts](../../../artifacts/build-migration/t484/s3-message/contracts.json)
record the completed runs. A repeated selected build leaves output timestamps
unchanged. Masks8192,-1,01,abc,999999 and the x64 row reject with exit2. The
integration script parses. Its legacy CompilerDriver/Nasm options remain
accepted for caller compatibility but no longer select or invoke compilers.

No original guest, product code, runtime assertion or private-desktop scope is
changed. No new guest runtime pass is claimed. The equivalent probe bytes reuse
the owner's prior-coverage authorization. Cached full host build and dual
publication identity checks are recorded separately; unchanged images require
no recopy.

B0045 remains pending: its logon helper compilation and shared runtime staging
still need migration. This batch removes the NASM/CL producer path, not the
whole transitive interpreted chain. S3, S4 and S5 remain open.
