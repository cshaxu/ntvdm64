# N021 executive and LPC headers - S15 P24

Starting HEAD 9f1e7bb26. Fixed XP SP1 object tree
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89:Source/XPSP1/NT/`.
N021 DIR positions 9-10 are ntexapi.h and ntlpcapi.h; nt.h is their existing
entry-point dependency. Before bytes, raw XP objects, omissions and compilation
logs are retained in build/t485-s15/n021-executive.

## Original restoration and necessary boundary

Restore the full XP executive declaration framework, including unused boot,
firmware, profiling and debugging declarations. Restore LPC original layouts
and both original includes in nt.h. Declarations alone do not introduce a
CSR/LPC transport, kernel execution or new service implementation.

The first real native-service-layout compilation of the complete header failed
on duplicate declarations from the selected Windows SDK and the B native-event
ABI. Retain whole-unit omissions only for the declarations listed in the
receipt: eleven SYSTEM types, two event types and three SDK query prototypes. The
actual compiler diagnostics are in probe.log; probe2 compiled successfully
after resolving these prerequisites. SDK-owned types remain supplied by
winternl.h; the event interface remains owned by B and must not include C.
The three SDK query declarations already exist;
their duplicate XP imported declarations produce C2375 linkage conflicts.
NtQueryEvent instead restores the original prototype: the B declaration now
uses NTSYSCALLAPI for its existing native DLL import, removing the conflict.
No second implementation or substitute executive algorithm is introduced.

The B support header supplies the missing original TIMER_TYPE, KPROFILE_SOURCE,
INTERFACE_TYPE, BUS_DATA_TYPE and PHYSICAL_ADDRESS declaration prerequisites.
Each retains the original names, members and scalar shapes, with upstream
file/line attribution. The selected Windows SDK supplies the other foundational
types; wholesale inclusion of the XP kernel and I/O implementation is not
required. HARDERROR_MSG now uses the restored original LPC PORT_MESSAGE.

Preserve three existing executive exclusion branches (_MAC, REMOTE_BOOT,
USER_SHARED_DATA) and two LPC USE_LPC6432 arms. Restore both original DEVL
sections: the complete product build proved DEM selects DEVL, invalidating
the old blanket exclusion. The initial failed build is retained as evidence.
Restore original C++ linkage rather than retaining the old unsupported-C++
exclusion. Removed original units retain diagnostic #line anchors. Audit
selected code mappings separately from non-executed excluded branch bodies.

## Accounting and delivery

Count raw-byte differences and nonerror differences separately. Keep LF, EOF
and #line in the latter; exclude only the registered unsupported branch bodies.
The two newly migrated headers switch individually from OpenNT to XP; nt.h
already uses XP. No mirror file is added or removed, and no guest changes.

Final counts and delivery evidence are recorded below. Required delivery is cached
full build, dual publication and hashes, native CSR capture/layout, service
lifecycle and actual guest COMMAND/NetAPI, then commit and push. This record
does not claim that the successful compile probe completes product acceptance.

## Consumer and residual audit

The original nt.h umbrella reaches the worker DEM/CCPU compilation, BaseClient,
BaseSrv and native CSR layout fixtures. LPC supplies PORT_MESSAGE to both CSR
and HARDERROR_MSG. The B event declarations also serve native frontend event
queries; therefore they cannot be replaced by a B-to-C include. NtQueryEvent
has no project definition: these consumers resolve the native DLL symbol.
Its restored imported prototype changes no request or wait policy.

The current candidate nonerror counts are nt.h 13/41 -> 14/40,
ntexapi.h 43/508 -> 20/302, ntlpcapi.h 4/4 -> 3/3. These compare the prior
registered per-file baseline against the new XP baseline. Raw XP differences
are respectively 16/44, 23/347, 5/11. The necessary #line anchors remain
counted. Matched retained nonempty source lines have zero diagnostic-line
mismatches, excluding directive lines within rejected branch arms.

Global candidate:1324 files,775 byte-exact,786 nonerror identical,
538 nonerror different,7718 points/152823 lines. Delta0/0/0/0/-23/-208.
Baseline population1254 XP and70 OpenNT. These measurements were verified against final delivered source hashes.

The second complete build exposed one additional DEVL type collision:
SYSTEM_INTERRUPT_INFORMATION is already defined by winternl.h. Retain only
that complete-type omission; keep every other restored DEVL declaration.
The failure and final correction are retained in build-final.log and
build-r3.log respectively.

## Final delivery

Cached full build passed. Runtime25 was dual-published:13 host images and26
verified destination hashes. x86/x64 native capture and layout, all service
lifecycle cases and real guest COMMAND/NetAPI passed against final inputs.
[Receipt](n021-executive-result.json); [global census](n021-executive-census.json).
No Full repeat: eight audited files since P21, six changed distinct mirror
files. N02110/17 delivered; next ntpsapi.h. Overall56/159, S15 8/14 directories.
