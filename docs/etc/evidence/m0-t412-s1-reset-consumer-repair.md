# T412 S1 approved RESET-consumer prerequisite

## Scope and source recovery

The owner approved the bounded event-consumer prerequisite after the
[S1 audit checkpoint](m0-t412-s1-broker-owner-audit.md). This record supersedes
its pending-approval disposition, not its historical observations. S1 remains
active; no broker production implementation or new T/S was started.

Reference revision is `1270035e5`; original HALT at OpenNT
`base/mvdm/softpc.new/base/ccpu386/c_main.c:3987` observes RESET without
consuming it. The original normal handler consumes it before c_cpu_reset.
The local CAS helper had accidentally made the HALT observation destructive.

Four-rung disposition: (1) original observe/consume order is usable and selected;
(2) keep the existing atomic snapshot instead of a racy native RMW; (3) no new
overlay/external intrusion; (4) no newly authored CPU policy. Production change
is one condition plus two explanatory comment lines under existing DIV-214.
There is no decoder, opcode, reset-body, BIOS, PIC or guest-memory change.

## Whole event-consumer profile review

- HALT RESET now atomically observes; normal RESET consumes once and runs
  the original c_cpu_reset body. HALT INTR already observes without consuming.
- HALT/normal timer and SAD consume before callbacks, retaining original
  consumer roles. Atomic clearing preserves unrelated event bits.
- Hardware IRQ still consumes the notification latch before ica_intack,
  unlike the original post-ack clear. This is explicitly retained, not called
  original-order equivalence: a raise during acknowledge must not be erased
  by a subsequent clear. Original ICA remains vector/priority authority;
  signed -1 rejection still prevents fabricated vector FF delivery.
  Source inspection includes ica_accept, ica_intack, ica_interrupt_cpu and
  host_set/clear_hw_int. Callback-boundary tests below do not prove all PIC,
  AEOI, EOI or concurrently changing device-state interleavings.
- No direct instruction, stack, string or memory-access algorithm changed;
  those CPU profile groups are outside this notification-consumer correction.
  The real-machine test nevertheless executes ordinary guest instructions,
  SAS storage, HALT, original reset and original D6 FE unsimulate.

## Reproducible tests

Build root: `build/M0-T412/S1/event-profile/`. The formal x86 graph selects
131 CCPU source files and completed the 439-step original product compile/link.
Subsequent graph regeneration added a test entry, preserving the same compiler,
flags and production sources; existing object reuse was intentional. Ninja
rebuilt changed fixture/resource/dependency outputs. Baseline c_main uses a
distinct generated source, object, library and product name, never overwrites
the production source. Raw compile/fixture output stays under this build root.

Commands:

```text
node tools/audit/Verify-CcpuEventConsumption.mjs 1270035e5
node tools/audit/Verify-CcpuEventConsumption.mjs
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build/New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T412/S1/event-profile/product -NodeExecutable O:/.nvm/versions/node/v22.22.1/bin/node.exe -ParallelJobs 4
cmd /d /c build\M0-T412\S1\event-profile\product\run-ninja-parallel.cmd original-softpc-process.exe ccpu-halt-reset-test.exe
node tools/audit/Verify-CcpuHaltReset.mjs 1270035e5
```

The source-extracted test uses the actual local atomic helpers and consumer
blocks, compiled as x86 with MSVC /MT. Pre-fix execution fails at HALT RESET;
fixed execution passes reset once, preservation of another bit, timer/SAD,
IF gating, rejected IRQ and reassertion during acknowledge. Its PIC callback
is controlled test input, not the original PIC: this is a focused boundary
test, not whole-machine evidence. Node used to orchestrate these tests was
24.19.0; the formal source generator explicitly used Node 22.22.1.

The second test links the same formal original CCPU/SAS owner libraries as
the product, with a fixture main and no substitute executor/reset provider.
Guest writes a marker and executes HLT; another thread raises CPU_HW_RESET.
The reset vector loads AX=BEEF and returns through original unsimulate;
the unreset continuation loads AX=DEAD. The script verifies that pre-fix and
current c_main differ only by the audited HALT repair and checks x86 PE types.

| Actual original-machine case | Observed result |
| --- | --- |
| Pre-fix source | exit 1; AX=dead, producer=0 |
| Repaired source | exit 0; AX=beef, producer=0 |

The reset producer waits for the guest marker then 50 ms. This provides a
reproducible bounded HALT test, not exhaustive timing-interleaving proof.
The fixture owns a synthetic ROM image in its own process, populated through
original SAS backing; it does not change product ROM protection or guest media.
Its early ordinary SAS stores could not overwrite formal ROM, so an initial
attempt ran BIOS/port code and faulted. Correcting fixture image loading—not
CPU semantics—made the intended experiment executable. The baseline also needs
NOP padding after HLT so its wrong continuation reaches the DEAD assertion
instead of misaligned instructions. Both A/B cases use the same final fixture.

## Real-program regression and publication decision

Existing `Generate-ObservationNinja.mjs` built the original Console observation
container. It was invoked with the real positional product grammar, not the
obsolete -f/-o/--command wrapper. Reports are under `O:/ntvdm64/logs/`:

- `t412-s1-reset-mem.txt`: product exited zero; real MEM output observed.
- `t412-s1-reset-command.txt`: COMMAND.COM /c ver exited zero and displayed
  MS-DOS Version 5.00.500.
- `t412-s1-reset-edit.txt`: EDIT startup text appeared, but the 4096-byte
  Console snapshot was corrupted (729 NUL bytes). Observation timed out at
  10 seconds and ended only its own test child; timeout is not application
  completion, keyboard or mouse acceptance.

MEM showed an implausible largest-program-size value. The old published EXE
produced a byte-identical MEM snapshot, so this is not newly introduced by
the candidate; the smoke result does not prove complete memory-report semantics.

Three-way EDIT comparison uses the same runtime media, observer and timeout:

| Product | SHA-256 | EDIT snapshot SHA-256 / NUL count |
| --- | --- | --- |
| Previously published EXE | C0771B414F93B1F1644765447F89439860E23E6F9FB67A1319222955B267652F | BC26704CAA93719DB9C34797D1C990C66BAD591E20822DD75071BFA398AD0E35 / 0 |
| Formal repaired build | 824DA0F8E93FEE75717AA01893902C079A4E834BD3308E4A1B430B9BABDAF573 | A54C555EFEC9F961716B733896F598C3205D1DBFAA04E618E358CADD477D8643 / 729 |
| Same formal graph, pre-fix c_main | B8881C41C084427942E46242DBA5B75085505D1B2FA60945915FC1FDCF9EE99C | A54C555EFEC9F961716B733896F598C3205D1DBFAA04E618E358CADD477D8643 / 729 |

Old-product reports are `t412-s1-before-mem.txt` and
`t412-s1-before-edit.txt`; same-graph pre-fix report is
`t412-s1-rebuilt-before-edit.txt`. All observers returned zero even when the
product timed out, so acceptance uses report contents and snapshots, never
the observer exit alone. Report image-identity was unavailable; independent
pre/post staging SHA-256 provides identity instead.

The identical corrupted snapshots in the same-graph A/B isolate the observed
display problem away from this single HALT edit in these runs. They do not
identify which earlier source/build difference causes it. The old deployed
binary is not a proven build of current HEAD.

Publication is withheld. Both `build/output/ntvdm32.exe` and
`O:/ntvdm64/ntvdm32.exe` were restored to C0771B...652F via the staging tool.
The saved original is `build/M0-T412/S1/event-profile/ntvdm32-before.exe`.
Candidate products remain under build for audit, not user acceptance.

## Disposition

HALT repair has focused and actual original-CPU A/B proof and is suitable for
a bounded source delivery. Full product acceptance and S1 closure remain open.
The newly isolated current-build versus published-binary EDIT regression is a
named prerequisite gap, not broker policy and not permission to rewrite video.
Source/artifact lineage diagnosis can proceed read-only; any additional C-VID/
Console implementation requires an explicit bounded scope decision. Remaining
broker D10, Console identity, resource transfer, WOW boundary and per-hunk
design/count work is not marked complete by this repair.
