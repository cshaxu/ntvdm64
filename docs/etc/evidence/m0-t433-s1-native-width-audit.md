# M0 T433 S1 native-width audit — initial findings

## Question, inputs and procedure

Owner expands S1 to include the single NTVWM alongside run16/NTCON/NTMON.
This record is initial source/link evidence, not S1 closure or x64 runtime
proof. No production implementation, compiler experiment or package change
has occurred in this stage.

Inputs: delivered b97041e74 source, sealed T432 S6/r048-runtime, current
formal x86 cache build/M0-T427/S2/r001 and Hook64 S6/r002-hook64.
The build-only audit entrypoint is
build/M0-T433/S1/r001/audit-link-closure.ps1. It checks each formal EXE hash
against the sealed baseline before parsing its matching map and exact Ninja
link edge; generated link-closure.json records image/map/graph hashes and
actually selected archive objects. This prevents treating old dual-worker
subdirectory maps or merely named input archives as current evidence.

Command: powershell -NoProfile -File
build/M0-T433/S1/r001/audit-link-closure.ps1. Result: all four image/map
pairings pass. A first ad-hoc rg used Bash brace syntax in PowerShell and
failed parsing; corrected explicit paths supply the source review below.

## Actual original implementation selected

| Consumer | Original BaseClient archive objects selected | Original RTL selected | Initial migration implication |
| --- | --- | --- | --- |
| run16 | classifier.obj and client.obj from original base/win32/client/vdm.c; capture.obj from base/ntdll/csrutil.c | error.obj and environ.obj | Full local Base message/capture/environment boundary needs audit, not only the classifier. |
| NTCON | None | None | Project bindings and frontend/client libraries remain selected; shared archive names do not imply original VDM/RTL composition. |
| NTMON | None | None | Its finite broker-transport/common-rpc projection path has no original VDM client or machine closure. |
| NTVWM | None | error.obj only | Not a second VDM client. Rebuild native worker/common/worker-base/installer and finite error binding; do not import Base VDM execution. |

Original error.c is already compiled at AMD64 in the delivered Hook64 graph
through opennt_support_rtl.c's private TLS binding. This is reusable build/ABI
evidence, not proof that the whole worker is ported. Common native_image and
shared application_search already compile for both Hook widths.

## Concrete width boundaries and design direction

| Boundary / location | Current observation | Proposed bounded direction / proof still needed |
| --- | --- | --- |
| run16/native_launch.c:27 | Capability environment text uses %lx and casts ULONG_PTR to unsigned long. Windows ULONG remains32 on x64. | Full native-width formatting/parsing at all locator endpoints; authentication remains NTSRV-owned. Test upper-bit/trailing/overflow/invalid values without assuming real handles always use all64 bits. |
| run16/frontend_scope.c:24 | strtoul returns a32-bit unsigned long before conversion to ULONG_PTR. | Checked native-width integer parse, preserving the same locator syntax and no new authorization role. |
| ntcon/main.c:81–84 | Four bootstrap handles use wcstoul into UINT_PTR. | Validate producer and consumer together; preserve restricted inheritance and startup failure cleanup. NTSRV remains x86, which does not justify truncating the native parser. |
| original csrutil.c:102–112,234–235 | Historical local capture offset arrays are PULONG and store (ULONG)Pointer; allocations also retain four-byte rounding. | Determine live use and native alignment before selecting a facade. Project source search found no separate offset-array consumer; that alone does not certify the truncated metadata or alignment. Keep original capture source in place; no rewrite or low-address allocator chosen. |
| opennt_support_rtl.c:211; host-compat/include/nt.h:208–246 | NtCurrentPeb/NtCurrentTeb are explicit private support/TLS carriers populated from public APIs, not modern PEB/TEB casts. | Preserve those ownership boundaries; verify their actual original environment/error consumers and native layouts. Do not replace them with hard-coded host offsets. |
| base_classifier.h:SECTION_IMAGE_INFORMATION | TransferAddress is native pointer and stack sizes SIZE_T; shared declaration already serves Hook64. | Reuse, verify public native layout and same-machine fallback; keep full native metadata separate from DOS/Win16 classification. |
| nthook32-dll/legacy_classifier_arch.h | _X86_ is enabled only after native declarations, only for the original classifier's NE rule. | Reuse a classifier-local composition, never globally enable x86 declarations for the x64 worker/client. Full run16 client closure still needs separate proof. |
| common/protocol/service.idl | Native resources use typed system_handle attachments; task/connection fields are fixed-width/copied. | Generate architecture-local clients for the same RPC41 interface initially; prove x64 clients/x86 service and rights/rollback. Bitness alone does not require another wire protocol. |
| NTVWM execution/installer | Real target handles and copied packets already use native HANDLE and checked uint64_t values; Hook selected by actual target machine. | A single x64 worker uses the matching existing Hook and finite opposite-width installer; no worker-width reservation or second hidden Console policy. |
| test Sysnative paths | Existing64-target tests use Sysnative because the present launcher is x86. | Adapt test image selection to actual launcher ABI while verifying real target machine; an x64 launcher must not blindly use the WOW64-only alias. Do not change user search/launch syntax. |

These are source-proven hazards/design obligations, not claims of an observed
current x86 product bug or inevitable x64 runtime failure. HANDLEs versus
numeric counts must be reviewed semantically; not every DWORD/ULONG is wrong.

Further boundary check: base_rpc_client.c:1130–1134 explicitly discards its
capture argument; the project provider does not run original CSR pointer
rebasing. Selected vdm.c capture calls use CountCapturePointers=0. Thus the
recorded ULONG pointer metadata is not currently a broker wire/resource
identity, and its existence alone does not establish a required mirror rewrite.
Actual message payload pointers, bounded allocations/string alignment and the
full selected RTL environment closure still need native-width verification.
Keep this source fact distinct from a compile/runtime pass.

## Current design and next work

Retain one ntvwm.exe, migrate it to x64-only if the bounded closure verifies.
Final intended ten-image package has five AMD64 and five I386 images; NTSRV,
NTVDM, WOW32, VDMREDIR and Hook32 stay I386. Build common/RPC and worker-base
for each consumer ABI; never link AMD64 worker-base into x86 NTVDM.
Preserve hidden Console, input and publisher ownership, real direct target
wait/result, NTSRV KEEP/RELEASE, final I/O and double endpoint ACK.

Remaining S1 work: full local capture/provider and RTL-environment dependency
audit; native syscall/structure sizes and alignment; complete frontend callback
and wire/resource ledger; exact construction/rollback contracts and detailed
positive/negative integration matrix. No x64 runtime capability is claimed.
