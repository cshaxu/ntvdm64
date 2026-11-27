# S14 P7 bounded H/N001-N003 omission repair

Admission follows N009 P6 commit 0262e23b1 and runtime8 Full24/24.
S13 closure and its 1,039-file hash reconciliation remain authoritative;
this repair does not reopen that population. The protected ntcsrmsg.h remains
byte-exact XP. No BaseClient source, original guest or winnt.zip is changed.

Reference: O:/repos.external/winxpscodes, commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, raw Git objects.
[Per-file objects, hashes, metrics, exclusions and line directives](p7-omissions-metrics.json).
Raw snapshots and final diffs: build/t485-s14/p7-omissions.

## Per-file result

Numbers are points/changed lines relative to XP, not source-file size.
Raw includes excluded pragma replacements; normalized nonerror removes only
matched original #error bodies, preserving conditions and #line records.

| File | Raw before → after | Normalized nonerror before → after | Result |
| --- | --- | --- | --- |
| src/mvdm-host/softpc.new/base/debug/markfrom | 1/72 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/base/ntdll/csrutil.c | 1/1302 → 5/240 | 9/500 → 5/240 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/public/sdk/inc/ntcsrdll.h | 1/399 → 0/0 | 7/59 → 0/0 | XP byte-exact |
| src/non-mvdm/base/ntos/ex/probe.c | 1/396 → 3/8 | 2/5 → 2/5 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/base/ntos/rtl/environ.c | 1/1851 → 2/7 | 1/1 → 1/1 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/base/ntos/rtl/error.c | 1/498 → 2/8 | 1/1 → 1/1 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/base/ntos/rtl/generr.c | 1/2815 → 1/9 | 1/9 → 1/9 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/base/ntos/rtl/i386/largeint.asm | 1/1710 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/base/ntos/rtl/i386/movemem.asm | 1/2655 → 1/1 | 1/1 → 1/1 | Necessary binding/alignment or exclusion/line mapping retained |
| src/non-mvdm/public/sdk/inc/ks386.inc | 1/2438 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/public/sdk/inc/ntrtlpath.h | 1/662 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/public/internal/base/inc/status.h | 1/438 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/sdktools/generr/generr.c | 1/864 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/public/sdk/inc/callconv.inc | 1/606 → 0/0 | 0/0 → 0/0 | XP byte-exact |
| src/non-mvdm/public/sdk/inc/ntcsrmsg.h | 0/0 → 0/0 | 0/0 → 0/0 | XP byte-exact |

The byte-exact set includes markfrom omitted from the old census, the complete
XP ntcsrdll.h, six N003/associated formatting originals. callconv.inc is a
separate direct association, not one of the nine N003 imported files.
ntcsrmsg.h is protected verification, not a repaired file. Fourteen files change.
Original LF exposes separated residual blocks; raw block splitting is not new
functional adaptation. EOF is included in raw byte comparison.

## N001 whole-file disposition

- XP lines 1–21: original banner retained.
- Line 22 csrdll.h: replaced by explicit public NT/LPC/message declarations,
  existing B local heap, native RtlRaiseStatus declaration and zero capture tags.
  csrdll.h imports private CSR connection globals and initialization; importing
  it does not supply a valid current-product CSR connection. Zero tags match
  the existing untagged local heap. No new heap or state machine.
- Lines 24–217 CsrClientCallServer: unconditional omission retained and visible.
  It remaps captures by CsrPortMemoryRemoteDelta, uses CsrPortHandle with
  NtRequestWaitReplyPort, and can call CsrServerApiRoutine in a server process.
  These are CSR-private connection semantics, not the current BASE transport.
  src/compat/include/base_client.h explicitly maps selected C calls to
  NtvdmBaseClientCallServer; src/compat/base_dispatch.c owns that provider.
  Calling the host NTDLL export instead would use the host OS CSR protocol.
  No fake #if/#error was added around this unconditional omission.
- Lines 219–242 CsrGetProcessId: unconditional omission retained and visible.
  Returns CsrProcessId initialized from ConnectionInformation.ServerProcessId
  by XP csrinit.c:500. That is the CSR server PID, not the caller PID or NTSRV
  PID. No selected consumer or CSR connection initializer exists here;
  fabricating that ID or silently replacing it with another process is invalid.
- Lines 244–754: complete XP allocation/free/pointer/message/string/timeout
  sequence retained, including restored single and multiple Unicode capture.
  Only two pointer-width alignment expressions differ. x64 capture records
  store pointer-width offsets; XP's fixed four-byte rounding is insufficient.
  Existing B CsrPortHeap supplies process-local HeapCreate/HeapDestroy lifetime.
- Lines 756–887: complete original write/read probe algorithms restored.
  Write probe's private CsrNtSysInfo upper address is replaced by current-process
  GetSystemInfo.lpMaximumApplicationAddress. XP base/win32/client/process.c:6580 assigns this public field directly from BasicInfo->MaximumUserModeAddress, proving the source-level correspondence. No CSR startup or cached state.
  Read probe is exact original. RtlRaiseStatus binds the existing public host
  NTDLL API; the first build proved its explicit prototype was required.
- No original conditional implementation branches occur in csrutil.c, so no
  new branch exclusions are manufactured. Original contiguous source from
  line244 is retained; one entry #line244 and #line790 after the added local
  query preserve diagnostics. Redundant per-function #line directives removed.
- ntcsrdll.h: entire XP object restored. Removed old callback structure and
  old signatures have no selected consumers. Public declarations do not import
  private implementations or force unreferenced symbols into product links.

## N002/N003 residual evidence

- probe.c: original complete ProbeForRead/ProbeForWrite retained; existing
  wow_user_probe_bindings supplies current user-address ceiling and native
  exceptions in place of kernel exp.h. Original ALLOC_PRAGMA guard retained;
  only kernel paging pragmas are #error-excluded. Active #line34 follows endif.
- environ.c/error.c: original runtime code retained. Kernel alloc pragmas are
  excluded under their original compound guards. #line34 moved outside the
  inactive guard so ordinary user-mode diagnostics actually use XP coordinates.
- RTL generr.c: rtl-error-bindings.h supplies the standalone host declarations
  instead of the seven kernel/build headers; original generator algorithm and
  generated error.h chain retained. #line30 maps the next original include.
- movemem.asm: existing MASM `option nokeyword:<sfence>` permits the original
  sfence macro name with the modern assembler; no instruction algorithm change.
- Remaining seven formatting files have zero normalized differences both before
  and after, now zero raw differences too. No exclusions were added or expanded.

## Verification and delivery

Final runtime9 cached full build and dual 13-image/26-hash publication pass.
Both capture architectures pass 3,475 checks, zero failures.
[Full24/24](p7-omissions-full.json) passed in one run, including real COMMAND,
DOS4GW, Win3.1 standard/enhanced, Win95, both enhanced fullscreen roundtrips,
and SOL Game > Options. Win95 retains the approved historical exit-one boundary.
[Delivery receipt](p7-omissions-result.json). No evidence item remains open
within this bounded repair. Source hashes match the tested final code.

First cached build failed only for missing
RtlRaiseStatus prototype introduced by restoring probes; second build passed.
Final build also verifies removal of redundant diagnostic directives.
Existing native_capture_test is extended for Unicode capture, sparse arrays,
invalid output argument, native pointer alignment, read/write probe data
preservation, zero length and misalignment exceptions, on both x86 and x64.
Fourteen changed audit files trigger Full; its COMMAND gates verify actual
product launch/exit. No original guest changes or recompilation.

Actual capture consumers remain BaseClient vdm.c and selected Run16/WOW paths;
raw reference search is capture-consumers.txt. Old callback and capture-pointer
ABI search has no consumers (removed-abi-consumers.txt). Both architecture
fixtures link the product capture.obj, not a copied implementation.
