# T431 controlled native launch plan

## Current admission revision

The owner closed S1's bounded contract/audit and admitted S2 on2026-10-05:
32-bit only, both CUI and GUI propagation, component `nthook32-dll`.
[S2 evidence](../evidence/m0-t431-s2-nthook32-implementation.md) records the
actual installer, production callers and tests. The following earlier S1
plan remains historical; its unimplemented/admission wording is not current
status. S3 cross-width is explicitly deferred by the owner and not
automatically admitted. Whole-package observation/fault coverage is not a
claim that every arbitrary CreateProcess attribute combination is supported.

Current reproducible entries:

- `nthook-install-test.exe`: actual CUI/GUI immediate children, actual x86 CMD,
  CreateProcessA, caller suspension, no-inherit private capability copies,
  explicit HANDLE_LIST/Unicode environment/CWD/streams, eight malformed
  payloads, invalid capability and missing DLL. Fixture events prove local
  transport/ownership only, not NTSRV authorization.
- `tests/observation/verify-native-hook-chain.ps1`: production SysWOW64 CMD
  absolute/bare COMMAND, parent-output return and explicit run16 via SUBST
  identity; optional WindowObserver verifies WINMINE startup receipt and
  the retained localized visible main window, not gameplay.
- Existing frontend scope and NTVWM execution lifetime fixtures retain actual
  receipt/authentication and uncommitted-target rollback assertions.
- ProductVerification Product/Full retain Console17/Window17 and independent
  WOW frontiers with the coherent nine-file set; no input or output assertion
  is weakened to accept Hook implementation.

CURRENT is the sole active packet. Owner admits the queue-head package after
T430 acceptance on 2026-10-05; the [proposal](../../proposals/proposal-native-launch-hook-001.md)
supplies the bounded sequence. No Hook production source root is created yet.

The [detailed design](t431-native-launch-hook-design.md) specifies the shared
suspended-child installer contract,64-byte copied bootstrap, real run16
context-only seed, no-inherit authorization, native passthrough and rollback.
Contract design is complete; four-width installation, actual CMD selection and
bootstrap runtime proofs remain explicit S1 gates. No S2 implementation is
auto-admitted by the design delivery.

Follow-up source audit identifies the exact32->64 blocker: unchanged Detours
and PR161 both reject it; the installed private WOW64 read/write/allocation
exports do not prove protection/undo or safe pre-entry installation. The
checkpoint pins the unmerged64->32 patch separately. A build-only private
WOW64 feasibility probe needs bounded re-admission before execution; no
helper, production change or reduced width coverage is authorized implicitly.

| Stage | Deliverable | Gate |
| --- | --- | --- |
| S1 | Source/API/bitness audit, one classifier/installer, initialization and supported-flag contract; [checkpoint](../evidence/m0-t431-s1-native-launch-hook-audit.md). | No-helper feasibility and authenticated inheritance/path contracts before production implementation. Currently open, not runtime-complete. |
| S2 | Actual Hook32 direct installation and controlled child propagation. | Real SysWOW64 CMD→COMMAND→return, nested/native passthrough and exact resource/error/exit semantics; affected tests/full product and coherent publication. |
| S3 | Hook64 and four width directions without helper. | Actual32→32/32→64/64→64/64→32; building DLLs is insufficient. Specific unresolved no-helper boundary requires reporting, not extra process or reduced acceptance. |
| S4 | Whole-package isolation/concurrency/fault/cleanup and trace handoff. | All established runtime gates, extended coherent runtime manifest and owner acceptance before T closure. |

## Reproducible capability case plan

All future executable fixtures belong below tests/ with outputs under the
admitted build/M0-T431 run. These names are planned cases, not existing tests
or passing evidence. Preserve all eight existing runtime files; hook-enabled
publication/recovery must add exactly the actually verified hook DLLs.

| Planned fixture/case | Assertions and prerequisites | Current status |
| --- | --- | --- |
| native-launch-hook-contract / dos-wow | Immutable COMMAND/MEM/EDIT and original Win16 image; confirmed type redirects once; DOS actual return and Win16 startup contract, native DLL/malformed/missing image do not redirect. | Not implemented. |
| native-launch-hook-contract / argument-resolution | Explicit/null application, quoted paths/ambiguous spaces, CWD/PATH, ANSI/Unicode, literal tail, no required --, no x86 System32-policy change. Compare unhooked native creation when applicable. | Not implemented. |
| native-launch-hook-contract / flags-handles | Suspended/no-inherit/handle-list, redirected file/pipe aliases, supplied environment, security/startup flags. Distinct fresh child markers prove consumption; no unintended handle escapes. | Not implemented. |
| native-launch-hook-contract / width-matrix | Four width directions, actual target machine and loaded DLL; unavailable/mismatched DLL and installer rollback; no helper/host mutation. | No-helper mechanism unresolved. |
| native-launch-hook-chain / real-cmd | SysWOW64/System32 CMD ordinary external COMMAND, nested CMD, DOS→native→DOS, Console/Window and native EDIT; original direct receipts and parent restoration. | Not implemented; imports only inspected. |
| native-launch-hook-isolation / roots-failure | Two independent roots, GUI/new Console boundaries, recursion/internal-role exclusion, parent/worker/broker death, concurrent and rapid child creation; no handed-off tree kill. | Not implemented. |

The existing original classifier/application-search/native-subsystem and
launch-options fixtures remain useful production-caller checks, but do not
prove injection or process propagation. Monitoring, Job observation, trace
record completion and NTMON hierarchy are outside this package.
