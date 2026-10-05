# T431 controlled native launch plan

## Current admission revision

Owner's latest S3 revision limits T431 to named ntvwm32/ntvwm64 and
nthook32/nthook64 variants. All other components stay x86, including
run16/ntcon/ntmon. Their x64 migration is the separate next unnumbered Queue
candidate, not an active S or a prerequisite. Native worker handoff reuses
the current protocol with private hidden Consoles. Ordinary native children
keep actual Windows process/handle/wait semantics, never Run16 mediation.
An installer-only transient helper is acceptable if necessary after design
review; no resident helper or replacement target. The
[frozen design](t431-native-launch-hook-design.md#s3-design-freeze-component-widths-handoff-and-native-propagation)
supersedes both the previous broad width freeze and earlier no-helper-only
options. S4 organizes only dual-worker/Hook builds and width selection;
S5 proves native propagation; S6 retains final audit.

The owner's follow-up S3 direction records the dual-build NTVWM option in
[the design](t431-native-launch-hook-design.md#dual-build-ntvwm-and-width-neutral-handoff):
each native worker owns its own hidden Console, and DOS/native32/native64
reuse the same NTSRV-controlled frontend handoff. No shared hidden Console,
NTCON worker-width branch or separate handoff policy is selected. Production
implementation/build admission and ordinary cross-width creation mechanics
remain explicit decisions; the published S2 package is unchanged.

Owner direction on2026-10-05 closes S2 before personal acceptance and admits
S3 for Hook64 architecture/source discussion. S2 P2 is12160c657, with coherent
r027-runtime publication and retained automated proofs. No personal acceptance
is claimed. The earlier cross-width deferral is superseded for discussion only:
implementation, a Hook-only x64 toolchain island or private WOW64 executable
probes require reviewed design and explicit admission. No helper or x64
MVDM/worker/service is selected.

The owner closed S1's bounded contract/audit and admitted S2 on2026-10-05:
32-bit only, both CUI and GUI propagation, component `nthook32-dll`.
[S2 evidence](../evidence/m0-t431-s2-nthook32-implementation.md) records the
actual installer, production callers and tests. The following earlier S1
plan remains historical; its unimplemented/admission wording is not current
status. S3 was deferred at that revision; its discussion admission above is
the successor. Whole-package observation/fault coverage is not a
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
| S1 | Source/API/bitness audit and contract; [checkpoint](../evidence/m0-t431-s1-native-launch-hook-audit.md). | Bounded source/design closed; runtime proofs assigned to S2/S3. |
| S2 | Actual Hook32 direct installation and controlled child propagation. | Owner-directed engineering closure at12160c657; automated tests/publication retained; personal acceptance pending. |
| S3 | Freeze component widths, shared native worker handoff and real native child creation/propagation design. | Documentation/source contract only; no64-bit runtime completion claimed. |
| S4 | Dual NTVWM32/64 and Hook32/64 source-ABI/build/package organization and worker selection; other components remain x86. | Mixed-width RPC/resources, x86 launcher selection of both native widths and Hook64 context-only delivery to x86 run16 preserve S2 behavior. General component x64 migration moves to the next T. Not yet admitted. |
| S5 | Hook64 and four-direction native propagation, preserving actual child identity. | Actual32→32/32→64/64→64/64→32 and rollback, including x86 run16 context delivery; reviewed transient installer helper permitted if necessary. Not yet admitted. |
| S6 | Whole-package isolation/concurrency/fault/cleanup and trace handoff (former S4). | All established runtime gates, extended coherent runtime manifest and owner acceptance before T closure. Not yet admitted. |

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
