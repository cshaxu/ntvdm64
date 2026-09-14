# opennt-host

This is the original non-MVDM OpenNT host-service mirror. It contains only
complete source-audited packages used by a selected MVDM caller. It is not an
alternate MVDM provider, a compatibility layer, or a general collection of
host utilities.

## Package admission and current package

This root is not limited to BaseSrv/BaseClient. Every separately admitted
non-MVDM OpenNT host package belongs below this root with its original
relative path preserved. A package is admitted only after the shared tracker
records its direct `mvdm-host` caller, complete outgoing boundary, final file
disposition and any required package-private adapter binding. Discovering a
same-named function body is not package admission.

`base/win32` is the first selected package slice. It contains the original
BaseSrv/client VDM command implementation and its direct original declaration
carriers:

- `base/win32/server/srvvdm.c`, `srvvdm.h`, `basesrv.h`, and `srvinit.c`;
- `base/win32/client/vdm.c` and `basedll.h`; and
- `base/win32/inc/base.h`, `basevdm.h`, and `basemsg.h`.

Every file above is byte-identical to its canonical selected OpenNT source
except registered true subsets required by a selected formal runtime.
The source selection, rejected OpenNT-4.5 `vdm.c` candidate and initial
interface dispositions are recorded in
[`m0-t291-s5-p2-basesrv-source-package-audit-001.md`](../../docs/etc/evidence/m0-t291-s5-p2-basesrv-source-package-audit-001.md)
and the shared MVDM/OpenNT host trackers.

`base/ntos/rtl/environ.c` is the selected D11 RTL slice. It is directly
reached by original `base/win32/client/vdm.c` and
`mvdm-host/dos/command/cmdenv.c`. Its source body was imported from
`O:\repos.external\OpenNT\base\ntos\rtl\environ.c` (source SHA-256
`6c67b0c09c40e9186ce28a735221634bade1d9402be085d965223092aa7d4f00`).
The selected body has only its three include/declaration seams changed:
`ntrtlp.h`, `environapi.h`, and `zwapi.h` retain the finite standalone
PEB/VM and private-symbol boundary; they provide no environment algorithm.
The imported file SHA-256 is
`4c0e0e870d97f351cd8b046c2901ba841b439c83b75fdb8a6e936b0c89cf2b4b`.

`base/ntos/rtl/error.c` and its generated `error.h` table are the selected
D12 RTL slice.  They are directly reached by Base VDM, COMMAND, DEM,
Redirector, NetLib and SoftPC error consumers.  They were imported from
`O:\repos.external\OpenNT\base\ntos\rtl\error.{c,h}` (source SHA-256
`88bf09ab98778e0ca58d9d232d3b7aba6115223f43eddf981f6e06b786e77e48` and
`179794837a8610488218e5e48768d4d79f3d160bb952c92b172d12f6807984d2`).
The imported contents differ only by the repository terminal-newline form.
The original table and mapping algorithm remain intact; the sole finite
standalone binding supplies the original per-thread `LastStatusValue` field.

`base/ntos/rtl/error.c` and its generated `error.h` table are the selected
D12 RTL slice.  They are directly reached by Base VDM, COMMAND, DEM,
Redirector, NetLib and SoftPC error consumers.  They were imported from
`O:\repos.external\OpenNT\base\ntos\rtl\error.{c,h}` (source SHA-256
`88bf09ab98778e0ca58d9d232d3b7aba6115223f43eddf981f6e06b786e77e48` and
`179794837a8610488218e5e48768d4d79f3d160bb952c92b172d12f6807984d2`).
The imported contents differ only by the repository terminal-newline form.
The original table and mapping algorithm remain intact; the sole finite
standalone binding supplies the original per-thread `LastStatusValue` field.

The package is a source mirror only at this point. It is not an enabled build
or runtime input until every reached CSR/private-host interface has a named,
same-shaped disposition. A later package-named subfamily under
`adapter-opennt-host` is permitted only for a private interface that the
owning original source proves cannot compose directly; it must not absorb
MVDM or generic Win32 semantics.

`netapi/netlib/ntstatus.c` is the second selected package slice. It is the
complete, byte-identical status-conversion unit directly reached by original
`mvdm-host/vdmredir/vrnetapi.c`. It preserves only the original
`NetpNtStatusToApiStatus` and `NetpApiStatusToNtStatus` algorithms; it does
not admit NetAPI services, RAP transport, NetBIOS, CSRSS, or a network
provider. Its identical three-source provenance and bounded disposition are
recorded in
[`m0-t324-s1-original-redirector-owner-binding-baseline-001.md`](../../docs/etc/evidence/m0-t324-s1-original-redirector-owner-binding-baseline-001.md).

`netapi/netlib/copystr.c` is the third selected NetLib slice.  It is the
complete, byte-identical OpenNT and OpenNT-4.5 conversion unit directly
reached by original `mvdm-host/vdmredir/vrnetapi.c` through
`NetpCopyWStrToStr`.  Its source-defined OEM conversion and bounded
destination contract remain in the original translation unit; it does not
admit a NetAPI/RAP transport or a replacement Redirector provider.  Its
secondary NetLib references are intentionally exposed to the formal link
audit rather than reimplemented in an adapter.

`netapi/netlib/allocstr.c` and `netapi/netlib/initoem.c` are the complete,
byte-identical OpenNT/OpenNT-4.5 direct NetLib support units reached by that
same `copystr.c` body.  They retain the original allocation, OEM-string
initialisation and error direction; their remaining NetLib dependencies are
likewise admitted only when the formal link audit proves a finite original
source path.

`netapi/api/apibuff.c` retains the source-identified `NetapipBufferAllocate`
true subset required by the selected `allocstr.c` unit.  It preserves the
original internal forwarding contract to public `NetApiBufferAllocate`; the
unreached RPC/MIDL allocator routines from the same original file are not
admitted merely to make the larger historical API product shell link.

## Divergence register

T412 S2 extends OPENNT-HOST-014 with the complete original GetNextVDMCommand
body (390 lines including its signature/comment), selected by
`OPENNT_BASE_CLIENT_VDM_COMMANDS` for the focused original-client/server graph.
The default product still selects the environment cohort; the command provider
is not yet selected in the deployed product. Source: pinned OpenNT revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`,
`base/win32/client/vdm.c`; LF-normalized restored function SHA-256
`47cf285cbc505d9d43a50af16e283ea51d1d9ef8d2ff093795086d0ca327addd`.
Only cohort guards and the finite declaration include are added around it;
the function body is unchanged. The same cohort now also restores original
ExitVDM, SetVDMCurrentDirectories, GetVDMCurrentDirectories, CmdBatNotification
and RegisterWowExec as one contiguous 320-line source group. Its LF-normalized
SHA-256 is `2936de23dcbbdec23cf9ea23416606a7a3faeb96473d407e4bfaee00f89ac3aa`,
from the same pinned source/revision. No additional outgoing CSR operation
shape is needed beyond the four declared bindings. See the
[S2 execution evidence](../../docs/etc/evidence/m0-t412-s2-original-lifecycle.md).

BaseUpdateVDMEntry is also restored unchanged in that cohort (57 lines;
LF-normalized SHA-256
`de56cfccd08922ae57df7332cc8df70f60db186f44f177336188c2d236e9fd26`).
Its process-reference to wait-handle update is exercised against the original
server. The independent `OPENNT_BASE_CLIENT_CLASSIFIER` cohort now restores
GetBinaryTypeW (211 lines) and BaseIsDosApplication (60 lines), unchanged from
the same pinned source. LF-normalized hashes are respectively
`f7fd9766ea08e676a2fd457fb3fa508384630640032ce0615f752dc8a43f9fbd` and
`9487407a2efc02764466cedbabb4e89424fd12d87d11601cce9fa5ad477f3711`.
Its finite private header renames the imported Kernel32 symbol collision,
uses native NT file opening and supplies image declarations. The path binding
requests the native full-path result and marks the original relative result
empty, retaining the original full-path branch without passing an undersized
NT4 structure to modern NTDLL. Classifier bodies/branches are unchanged.
The focused x86 graph selects these bodies; final run16 selection is pending.

| Identifier | Original purpose | Reason for divergence | Selected implementation | Files |
| --- | --- | --- | --- | --- |
| OPENNT-HOST-001 | `public/sdk/inc/ntexapi.h` declares the historical NT Ex system-service surface. | The selected non-MONITOR `softpc.new/host/src/nt_eoi.c` preserves its original include, and `nt_timer.c` reaches only its performance-counter/tick declarations. Importing the complete header would falsely enlarge the user-mode host closure. | Retain the source-identified true subset as the same-named OpenNT declaration carrier. The selected MVDM sources remain unchanged; no Ex service, controller, or kernel-VDM substitute is supplied. | `public/sdk/inc/ntexapi.h` |
| OPENNT-HOST-002 | `public/sdk/inc/ntpsapi.h` declares historical process/thread services. | Reached `nt_timer.c` needs only `NtAlertThread(HANDLE)` through the broad NT4 platform include closure; importing all process services would falsely enlarge the modern host boundary. | Retain that one byte-identical declaration. Its session-owned cancellation implementation belongs to the named MVDM host-out adapter, not this source mirror. | `public/sdk/inc/ntpsapi.h` |
| OPENNT-HOST-003 | `public/sdk/inc/ntrtl.h` is the original NT runtime declaration carrier. | The complete NT4 header reaches private product declarations outside the selected user-mode closure. | Retain a source-identified true subset of declarations reached by imported MVDM/OpenNT source; implementation binding remains in `adapter-mvdm-host-out/win32`. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-004 | `ntrtl.h` supplies the historical `DbgBreakPoint` declaration. | Modern SDK selection does not expose this exact declaration consistently. | Preserve only the source-facing declaration; the debugger adapter owns any enabled behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-005 | `ntrtl.h` supplies the NT4 `TIME_FIELDS` layout and time-conversion declarations. | The selected modern include route omits this small public-NTDLL subset. | Preserve the original layout/declarations only; no CMOS policy or host behavior is added. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-006 | `ntrtl.h` supplies SoftPC timer RTL arithmetic, critical-section, and process-control declarations. | The selected modern SDK does not supply a coherent source-compatible declaration set. | Preserve the reached signatures and original `(HANDLE)-1` pseudo-handle spelling; an adapter must provide any non-exported behavior. | `public/sdk/inc/ntrtl.h` |
| OPENNT-HOST-007 | `public/sdk/inc/nturtl.h` carries NT runtime utility declarations. | Its full historical source closure is broader than the selected MVDM callers. | Retain only the reached declarations; this mirror adds no filesystem or process behavior. | `public/sdk/inc/nturtl.h` |
| OPENNT-HOST-008 | `ntpsapi.h` declares the reached `NtAlertThread(HANDLE)` surface. | The source-shaped public adapter must provide the historical name locally; ordinary `NTSYSAPI` import decoration would force an unavailable/private NT4 import instead. | Retain the original signature and select a local declaration only in the named adapter-backed SoftPC build; other consumers retain the historical declaration form. | `public/sdk/inc/ntpsapi.h`; `../adapter-mvdm-host-out/win32/{include/nt_thread_alert_compat.h,source/nt_thread_alert_compat.c}` |
| OPENNT-HOST-009 | `public/sdk/inc/ptypes32.h` carries the small historical 16/32 portability declaration set used by original SoftPC serial sources. | `nt_com.c` and `nt_wcom.c` directly include it, while the modern SDK has no same-named carrier. | Preserve the byte-exact selected OpenNT public header (SHA-256 `fbc65366df6b091aadbdc0ce12eb842bbd48691ccb1b9ce34c6d503226a7139b`); it contributes declarations only and no serial provider behavior. | `public/sdk/inc/ptypes32.h` |
| OPENNT-HOST-010 | `ntrtl.h` declares the original mutable RTL environment APIs reached by COMMAND. | The selected declaration subset initially omitted this small source-facing cluster, leaving original callers to compile by implicit declaration. | Retain the original declaration shapes; selected consumers enter the private `OpenNtRtl*` symbol domain implemented by the admitted original D11 body, not modern NTDLL or an adapter algorithm. | `public/sdk/inc/ntrtl.h`; `base/ntos/rtl/{environ.c,environapi.h}` |
| OPENNT-HOST-011 | `ntrtl.h` declares `RtlOemToUnicodeN`, reached by original SoftPC keyboard conversion. | The selected true subset omitted this fixed-buffer OEM-to-Unicode form although the original `nt_keycd.c` includes and calls it. | Restore only the original declaration shape; the public Win32 implementation remains adapter-owned. | `public/sdk/inc/ntrtl.h`; `../mvdm-host/softpc.new/host/src/nt_keycd.c` |
| OPENNT-HOST-012 | `ntrtl.h` declares OEM conversion forms reached by original NetLib `copystr.c`. | The admitted true subset omitted the fixed-buffer conversion and size contracts although the selected original Redirector dependency includes them. | Restore only the original signatures; the selected x86 NTDLL/import-library binding is verified as part of the Redirector network closure. | `public/sdk/inc/ntrtl.h`; `netapi/netlib/copystr.c` |
| OPENNT-HOST-013 | `netapi/api/apibuff.c` provides historical public and internal NetAPI buffer entry points. | The selected Redirector NetLib closure reaches only its `NetapipBufferAllocate` compatibility forwarder; importing the other functions falsely pulls an RPC/MIDL product shell. | Preserve the original function unchanged as a source-identified true subset and call the public modern `NetApiBufferAllocate` contract. | `netapi/api/apibuff.c`; `netapi/netlib/allocstr.c` |
| OPENNT-HOST-014 | `base/win32/client/vdm.c` supplies `BaseCreateVDMEnvironment`, `BaseDestroyVDMEnvironment`, and `BaseGetEnvNameType_U` before original Base creates an NTVDM child. | The full source file pulls the excluded Base/CSR process-creation product shell, while the selected CLI needs this exact environment projection before its direct SoftPC entry. | Preserve the three original bodies as a source-identified true subset. Its clone uses the admitted original D11 RTL body; the separate adapter owns only current-process installation/restoration around direct entry. | `base/win32/client/vdm.c`; `base/ntos/rtl/environ.c`; `../adapter-mvdm-host-out/win32/source/mvdm_base_vdm_environment.c` |
| OPENNT-HOST-015 | `base/ntos/rtl/environ.c` provides original create/clone/query/mutate/destroy ownership for ordered UTF-16 MULTI_SZ environments. | Original MVDM/VDM callers require the complete D11 algorithm, but standalone cannot expose NT4's real PEB or private kernel VM service shell. | Preserve the selected body under its original-relative path. Its only outgoing closure is a private PEB projection, PEB lock, public user-mode NTDLL `Zw*VirtualMemory` declarations and private symbol names, supplied by the bounded adapter seams. | `base/ntos/rtl/{environ.c,ntrtlp.h,environapi.h,zwapi.h}`; direct consumers `base/win32/client/vdm.c`, `../mvdm-host/dos/command/cmdenv.c` |
| OPENNT-HOST-016 | `base/ntos/rtl/error.c` and generated `error.h` map NTSTATUS values to DOS/Win32 errors and record the reached status in the TEB. | Modern NTDLL exports the historical names, so it cannot be mixed with the selected original table under the public symbol names; the standalone process also cannot expose the NT4 TEB layout. | Preserve the selected original source/table under the original-relative path and route selected callers to a private symbol domain. The adapter contributes only the source-shaped TLS `LastStatusValue` carrier; it supplies no mapping entries or fallback policy. | `base/ntos/rtl/{error.c,error.h,ntrtlp.h}`; `public/sdk/inc/ntrtl.h`; `../adapter-mvdm-host-out/win32/{include/nt.h,source/opennt_support_rtl.c}` |
