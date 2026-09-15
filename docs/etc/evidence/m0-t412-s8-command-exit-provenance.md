# T412 S8 COMMAND exit-code provenance

## Question and disposition

Owner requested research/repair, build/test, publication, commit/push, then
wait. Baseline is pushed `accf1e865` (S8 admission), with S7 code at
`8335f38bd`. The apparent result corruption is **not a demonstrated host
divergence**. Both surprising results below are explicit original COMMAND
paths. No product repair or changed guest behavior is claimed. The subsequent
[owner acceptance](#owner-accepted-s8-closure) closes S8; T412 and its remaining
acceptance obligations stay open.

| Workload | Observed result | Original owner |
| --- | --- | --- |
| Interactive COMMAND, immediate EXIT | 0 | `tcode.asm`, saved Start16 |
| Interactive COMMAND, MEM then EXIT | 1 | `tcode.asm`, saved Return16 |
| COMMAND /c external child returning 7 | 0 | `command2.asm`, resident SingleCom termination |
| Direct DOS child returning 7 | 7 | DOS INT 21h/4Ch through original COMMAND/BOP and BaseSrv |
| Direct native child returning 7 | 7 | run16 native process result |

These are properties of the selected OpenNT source and accepted image, not
a claim about every Windows or COMMAND version. Making EXIT always zero or
making this COMMAND /c propagate RetCode would **change** those guest paths.
S8 explicitly excludes replacing/rebuilding guest media. A launcher override
would hide the guest result and is rejected.

## Inputs and source-first audit

Accepted load-only `O:\winnt\COMMAND.COM` SHA-256:
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
Guest source under `src/mvdm-guest/dos/v86/cmd/command/` is compared byte-for-byte
with `O:\repos.external\OpenNT\base\mvdm\dos\v86\cmd\command\`.
The retained image test checks `tcode.asm`, `command1.asm`, `command2.asm`
identity and the exact image bytes described below. No guest image changed.

1. `tcode.asm::exec_comspec` supplies AX=0100; `reent_ret` supplies AX=0101.
   `Do16BitPrompt` saves this entry AX. Recognized EXIT restores it and enters
   `reent_exit`, which preserves AX around cleanup, sets AH=4Ch and INT 21h.
   AL is therefore Start16=0 or Return16=1, **not the last child's RetCode**.
   Native command completion also clears RetCode at `d16_return32` without
   changing the prompt invocation's saved AX.
2. `command1.asm::Exec_Wait` calls INT 21h/4Dh and stores AX into RetCode.
   `command2.asm::LodCom1`, when SingleCom=-1, jumps directly to `FatalRet2`.
   That path sets AX=4C00 and INT 21h, despite a nonzero RetCode. The separate
   transient `$EXIT` code which loads RetCode is **not** the reached path.
   The label FatalRet2 alone does not prove a fatal error: normal SingleCom
   completion deliberately uses it.
3. `cmdmisc.c::cmdGetNextCmd` obtains `pCMDInfo->ReturnCode` and passes it to
   GetNextVDMCommand. Original `srvvdm.c` stores `b->ExitCode` in DOSRecord's
   ErrorCode before signaling the parent; BaseSrvGetVDMExitCode retrieves it.
   The finite RPC client copies this scalar. run16 waits before querying the
   original BaseClient result, or queries the process result where applicable.
   No source-proven zero/one conversion was found in these reached bindings.

Recovery ladder: **retain original owner**. Original guest implementations
already exist and execute; there is no missing function body to reimplement,
new mirror slice to import, or overlay to supply. Direct original source,
same-shaped adaptation, equivalent implementation and a new provider were
considered in that order: adaptation/replacement is unnecessary for preserving
the observed original contract. No new product mirror diff, overlay or
autonomous runtime implementation remains in S8.

## Machine-code and runtime proof

Disassembled the accepted image using MSYS2 mingw32 `objdump -D -b binary
-m i8086`. File offsets exclude the COM PSP; they are not physical addresses.

| File offset | Bytes | Meaning |
| --- | --- | --- |
| 2CA3 | B0 00 B4 01 | Start16 entry |
| 2D1B | B0 01 B4 01 E8 6E 00 | Return16 entry and prompt call |
| 2D90 | 06 50 | Save ES and AX |
| 2E45 | 58 80 FC 00 74 03 E9 01 FF | Restore AX and branch to reent_exit |
| 2D8B | 58 B4 4C CD 21 | Exit using saved AL |
| 0D4B | B4 4D CD 21 A3 A7 02 | Query child result and store RetCode |
| 0F79 | 83 3E B1 02 FF 75 03 E9 CB FE | SingleCom=-1 branch to FatalRet2 |
| 0E67 | B8 00 4C CD 21 | Explicit zero exit |

A diagnostic-only rebuild observed real-mode INT 21h entry registers and
read-only ES/DS RetCode values, without changing guest registers or memory.
`m0-t412-s8-int21-command-guest-seven.txt.command.log` records:

- child `0A3F:0105`, AX=4C07;
- COMMAND INT 21h/4Dh, `FFFF:5CDF`, DS=ES=071E;
- COMMAND exit `FFFF:5DFC`, AX=4C00, **DS/ES:[02A7]=0007**;
- final guest BOP result 0000 and launcher result 0.

The HMA relocation matches file offsets 0D4F and 0E6C by the same +4F90h
offset. A separate narrow write watch also recorded RetCode=7. Thus the
first observed change is original guest FatalRet2, not lost DOS status,
CCPU arithmetic, or broker conversion. All temporary `c_intr.c`,
`ccpusas4.c` and termination-observer instrumentation was removed before
the final build. These diagnostic builds are not the published baseline.

Correction of discarded evidence: initial G7.COM tests used a long build
path, which was truncated in the DOS-facing path and failed to open (error
3). Their result 0 is **not** evidence of executing G7 or losing its result.
Subsequent tests temporarily mapped unused Z: to the S8 build directory,
verified fixture identity and successful DEM open, and removed that mapping.
No media was installed to the package for these fixtures. General long-path
support is not resolved here.

## Reproduction and tests

Build root: `build/M0-T412/S8/exit-status`. Generated with
`tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86`, then built
with VS2022 BuildTools, x86 MSVC /MT and Ninja -j4. The fresh graph completed;
the final relink after diagnostic removal passed the VdmTib full-storage
gate: owner `mvdm_vdm_tib.obj`, 4208 bytes, no overlapping symbol.
Build stdout/stderr stays in that build root.

Retained checks:

- `tests/app/command_exit_image_test.mjs COMMAND.COM original-tcode.asm`
  (invoke with Node and the full input paths above): image/source proof.
- `basesrv-service-reservation-test.exe`: first command result 7 is queried
  after its parent wakes; a distinct later task returns 0, not stale 7.
  Log `O:\winnt\logs\m0-t412-s8-service-reservation.txt` passes.
- Build `tests/observation/console_startup_observer.c` with MSVC /W4 /MT,
  dbghelp.lib and user32.lib. The only observer change adds slash's set-1
  scan code 35h, enabling actual `cmd /c exit 7` keyboard input.
- `tools/audit/Verify-CommandExitStatus.ps1 -Observer <build observer.exe>
  -LogPrefix <fresh-prefix> -GuestFixturePath <mapped-drive-G7.COM>` runs actual run16
  and Console key injection. The test-only build fixture is B8 07 4C CD 21.
  Z: must refer to the build root and must have been unused before mapping;
  remove only that verified mapping afterward. The script refuses existing
  package processes, checks input delivery, successful fixture open, final
  guest/launcher agreement and native-child 7 where required. It only stops
  exact-path descendants of its recorded test launcher.

Final product matrix: `O:\winnt\logs\m0-t412-s8-final-c-summary.json`;
all 15 cases passed. No-child, VER, missing-command recovery and native-7
then EXIT return 0; MEM, repeated MEM, nested empty, two nested levels with
three MEM executions and EDIT return 1; direct MEM and COMMAND /c VER return
0; COMMAND /c native-7 and COMMAND /c DOS-7 return original zero; direct
DOS-7 and native-7 return 7. Each case's reports use the same prefix.
The script's expected /c result was corrected **after** source, image and
live-register proof; a changed expectation is not presented as a product fix.
The earlier final-a complete matrix and final-b repeat of empty EXIT,
post-MEM EXIT, direct DOS-7 and COMMAND /c DOS-7 also passed. Their ntvdm
hash was `0174AE34254C9CDC67F96898C3D3024B2A03EDF4F58D10CDE90E9DA409FF7F3E`.
Building the service assertion triggered another ntvdm relink: only four
bytes differed, in the PE timestamp at 0120h and debug-directory timestamp
at 283C2Ch. Nevertheless the newly linked file was republished and the
**entire** 15-case final-c matrix rerun against the hashes below.

## Publication and limits

Formal build products copied to `O:\winnt`, with build/deployment hashes equal:

| Product | SHA-256 |
| --- | --- |
| run16.exe | 90B28D61C414FB82BBF6E4FA0D11E1AE2A41E5A3CC5E0F93AEEC39CAC376DC29 |
| basesrv.exe | 5E005767D903FCCEA4990DFF23CDA2DA2F370A05F6D670B5F03E9B29D7C2E272 |
| ntvdm.exe | D8C12E679A4D0506E502A905EC3150D7F16D5A0810FF814E3C3345FE2AB2A647 |

S8 retains test-only observer/service assertions, a source/image check, a
runtime matrix driver and indexed evidence. Product-source delta is zero;
the guest is unchanged. This is a completed diagnosis and verification,
**not** a behavioral repair or full T412 closure. The owner subsequently
required retaining that original behavior, as recorded below. No next S/T is admitted.

Full WOW/WRITE recovery, redirection, MEM accounting and the remaining T412
matrix are neither retested nor closed by these exit-code controls.

## Owner-accepted S8 closure

Owner: “就按照原版语义来 不得改动 那么当前s任务就算收口啦？”
This accepts the source/image/runtime findings delivered at pushed `67ee292cb`
and closes S8. Original prompt EXIT and resident /c results remain unchanged;
neither guest patching nor host exit-code normalization is authorized.
The pending behavior decision is resolved, not deferred as an exit-code bug.

Closure relies on the existing 15-case final-c Console matrix, image/source
identity test, nonzero-then-zero service check, VdmTib gate and published hashes
above. This delivery changes governance records only: no source, build,
guest media or deployed executable changes, and no new runtime claim.
Documentation governance, relative links and diff checks are the closure gates.
T412 stays open between S packets; no next S/T is activated. Await owner direction.

## Closed S8 Packet

### M0 T412 S8 — COMMAND exit-code provenance and repair

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T412 S8; Ordinary Mode, one implementer. |
| Admission And Approval | Owner: “提交推送收口s任务 再准入一个新的s任务解决退出码问题”. S7 closes at its delivered nested-execution repair; unresolved exit status transfers explicitly to S8. Commit/push to origin/main remains approved. |
| Objective | Establish the complete COMMAND guest-to-parent exit-code contract and repair source-proven divergence; retain nested execution and faithfully propagate normal and nonzero results. |
| Non-goals | No forced-zero exit policy, worker reaper, CPU/video redesign, guest-media replacement, new parser, full redirection or WOW16 recovery. MEM size-accounting debt and the remaining full-T matrix are not closed here. |
| Reference Baseline | Pushed 8335f38bd and the three O:\winnt images identified in S7 evidence. Nested MEM/EDIT return works; explicit COMMAND EXIT returns 1, while direct MEM and COMMAND /c ver return 0. |
| Files And ABI Surface | COMMAND RetCode/WAITPROCESS/BOP, cmdexec/cmdmisc, BaseClient/BaseSrv results/waits, RPC and run16 completion. Guest source is comparison only. |
| Applicable Rules | Source policy; execution, architecture, coding and document rules; package-first recovery and original-owner placement. |
| Verification | Same-build x86: no-child/post-MEM/nonzero-child EXIT, /c, two nested levels, missing-command recovery, repeated runs, EDIT return, direct controls and VdmTib gate. Correlate guest RetCode through BOP/DOSRecord/native-child to run16. |
| Expected Markers | Proven first mismatch; per-task result preserved without stale/shared values, premature completion or forced success; responsive parents. |
| Asset Needs | Accepted local OpenNT source, immutable O:\winnt binaries, S7 source/artifact/log baseline; disposable build/M0-T412/S8/ run roots and O:\winnt\logs observations. |
| Reporting Requirements | Original owner/four-rung disposition, before/after result chain, negative tests, build/hashes, diff/overlay/autonomous footprint and limitations. |
| Stop Conditions | Missing provenance, unbounded platform dependency, contradictory selected-provider evidence, unproved mandatory contract or changed product scope: record the exact issue; do not manufacture success or a parallel provider. |
| Exit Criteria | Proven exit semantics, minimal repair, passing positive/nonzero/nested regressions without normalization; verified publication, reviewed evidence, commit/push and scoped test cleanup. Unresolved mandatory results block closure. |
| Original Owner Request | “提交推送收口s任务 再准入一个新的s任务解决退出码问题”. |
| Similar-Issue Sweep | RetCode initialization and lifetime, dwExitCode32 ownership, nested completion association, receipt/result conversion, wait-before-query ordering and failure paths; include mirrors, overlays and adapters in this bounded chain. |
| Candidate Proposal | [Broker design](../../proposals/proposal-cross-process-broker-closure-001.md). |
| Working Plan | [T412 sequential S plan](../operations/m0-t412-broker-restoration-plan.md). |
