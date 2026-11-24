# T484 S3 P176 shared DDEML environment reader

The current DDEML probe runner required an externally compiled C# logon helper.
It now builds the existing native logon-environment target through CMD and uses
the same shared reader as the current message/timer probes. LogonHelper remains
an optional compatibility argument, not a build or runtime input. Preserve the
complete user environment, source/DLL identity checks, private desktop, existing
assertions and cleanup. No original guest or product source is changed.

Actual run: build/t484-s3-ddeml-native, supervisor23488 and runner5748;
2026-11-24T10:13:04Z to10:14:01Z, exit0. Existing owner installation input
O:/w31/system/DDEML.DLL has SHA256
6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B.
The original is only copied into owned stages, never modified. Init asserts
invalidReserved=16390, init/reinit=0, identical nonzero instance, uninit=1 and
secondUninit=0. Register asserts callbackCount=2, registerCount=1,
unregisterCount=2, firstType=32930, lastType=32978 and successful registration,
unregistration, string release and uninitialization. Controls not selected in
this batch are not claimed as new runtime coverage.

Delivery PID24084 completes cached full and11 raw/22 published hash checks in
61.5seconds, exit0. Published bytes are unchanged, so no redundant publication.
Shared runtime staging already invokes the native package-ID tool, but retains
PowerShell copy/manifest orchestration; S4 must decide/migrate that shared
staging boundary. B0043 therefore remains pending, now only for runtime staging;
314/355 disposed,41 remain. The environment-reader migration is delivered,
not whole-chain closure. Governance: closure-result.json in the same batch.
