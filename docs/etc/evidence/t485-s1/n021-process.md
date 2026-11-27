# N021 process/thread declarations - S15 P25

Starting HEAD bf2beb55a. N021 DIR position11 is
`public/sdk/inc/ntpsapi.h`. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, blob
`3bf8c5fb976bba9e331743966b9d75c68cafad3e`.
The preceding OpenNT blob is `e48aa444b4fe4a3ed0bcc58d3bb313e1718df4c3`.
Before/raw XP bytes and actual compiler logs are under
build/t485-s15/n021-process.

## Whole-file audit

Restore original XP constants, declarations, C++ linkage, architecture
conditions and original LF bytes. Unused declarations are retained. This
header changes declaration ownership; it does not add process policy or
replace guest execution. Actual consumers include the nt.h umbrella,
BaseClient vdm.c, BaseSrv and CSR native layout tests.

The raw XP header was compiled with real product parameters. probe.log
records duplicate SDK declarations and the missing original pebteb.h.
The exact original pebteb.h was then supplied for probe2: its PEB and TEB
definitions also conflict with the selected SDK. The temporary candidate
was removed after recording this evidence; its original bytes remain in the
audit directory. No new dependency file is admitted merely to duplicate
native host types. The existing host carrier remains independently owned.

Omit only whole declaration units identified in omissions.json: SDK process,
thread, job and TIB types; the three NtQueryInformationProcess,
NtQueryInformationThread and NtSetInformationThread declarations whose import
linkage conflicts with winternl.h; and the pebteb.h include. Native-service
layout compilation then passes. No substitute algorithm or compile-time
selector is introduced for these omissions.

The NtCurrentPeb user macro was separately tested against the actual
BaseClient target. peb-probe.log records C2039 for ProcessEnvironmentBlock:
the imported client uses the B host carrier, not a raw historical TEB.
The B NtCurrentPeb implementation refreshes current directory and standard
streams through GetCurrentDirectoryW/GetStdHandle. Restoring the raw XP
macro would also bypass that refresh; adding a pointer alone is not equivalent.
Keep both one-line macro omissions and original line mapping. The complete
product build proved RTL environ.c selects NTOS_KERNEL_RUNTIME, whose XP
macro calls the unavailable kernel PsGetCurrentProcess (C4013/C2223). Both
selections use the same existing B carrier; the original conditional framework
remains and no second host PEB path is added.

Preserve the two matching original excluded branches, IA64 and DBG, with
#error and #line. Old NT4-only conditional exclusions absent from XP do not
create invented XP guards. All matched retained nonempty source lines have
correct original diagnostic mappings; directive lines within rejected arms
are evaluated separately. No guest source or binary is changed.

The x86 final build also proves a PPS_POST_PROCESS_INIT_ROUTINE calling-
convention collision with winternl.h (C2373); retain the SDK typedef and omit
only that original duplicate unit. This does not add a callback implementation
or an architecture-specific conditional. build-r2.log retains the evidence.

## Difference accounting

Before: OpenNT nonerror1point/2292lines. Candidate: XP nonerror32points/280lines,
delta+31points/-2012lines. Raw XP comparison is34points/296lines.
The previous whole-file newline difference concealed separate declaration
omissions; original LF restoration exposes them. This is not a claim of a
point-count reduction. The owner permanently approved this formatting case.
The complete omission list retains exact XP ranges and concrete evidence.
For explanation only, newline-normalized nonerror comparison is49points/358
lines against old OpenNT and32points/280lines against XP. Thus the separated
adaptations also decrease; these normalized values do not replace the required
byte-sensitive global census. The two upstream baselines remain explicit.

Global candidate:1324files,775byte-exact,786nonerror identical/538different,
7749points/150811lines. Delta0files/0exact/0same/0different/+31points/-2012lines.
1255 XP and69 OpenNT baselines; one individual baseline transition, no file
addition/removal. All original bytes, including LF/EOF/#line, remain counted.

## Delivery

Final cached full build and runtime26 dual publication passed:13 host images,
26 destination hashes. Both native capture fixtures pass3475 assertions,
both layouts pass, all42 service cases pass and actual guest COMMAND/NetAPI
returns NETAPI-OK. [Receipt](n021-process-result.json);
[global census](n021-process-census.json).
N02111/17 delivered; next ntregapi.h. Overall56/159, S15 8/14 directories.
Nine audited files since P21 Full24/24, seven changed distinct mirror files.
The next completed file triggers the ten-file Full checkpoint.
