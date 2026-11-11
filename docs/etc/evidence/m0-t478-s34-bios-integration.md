# T478 S34 BIOS integration investigation

Parent 005452de8; finding 21. Final closure and worktree-only publication are recorded below.
The owner authorizes minimal source-proven host corrections, preserving
original semantics without new tolerance, guest changes or O:/winnt writes.

## Confirmed D11 slave EOI defect

Both pinned OpenNT editions have the same `softpc.new/base/bios/slave_bs.c`
SHA256 22079CB4C1E033F1D33246DA6F60F8D5E1BBA5C925EE3E9E5EA3C3FD756C7368.
The original D11 body masks unexpected slave lines, then mistakenly writes
the non-specific EOI value to ICA1_PORT_1/A1h. Actual `ica.c` interprets this
as OCW1 (mask), not OCW2 (EOI). Its original sibling re_direct and int_287
already acknowledge the slave through ICA1_PORT_0/A0h. There is no missing
external provider or broader algorithm to import.

The background actual-object fixture links existing selected BIOS, IOS and
PIC archives (map recorded), issues actual ICWs, raises hardware requests,
and calls real INTACK before D11. The external host EOI callback only counts
notifications; it does not implement the PIC or manufacture an EOI. Existing
bounded CPU test seams remain, so this is not real guest/CPU IRQ execution or
concurrent lock verification.

Baseline `build/t478-s34/slave-before2` (runner PID 6636) exits 1 with
44 checks, 15 failures. On each of IRQ10/11/12/14/15, the slave ISR remains
04/08/10/40/80 respectively, IMR becomes 20h, and no EOI callback occurs.
No-ISR, master-only, AEOI, IRQ9 redirect and x87 acknowledgement controls pass.
Repeated service is deliberately not attempted after a failed first retirement;
the fixed run must complete two rounds per line without a fabricated EOI.

The first attempt `slave-before` (PID 21268) was a rejected fixture setup:
it omitted original io_init and the existing fixture's post-readiness
DelayIrqLine=0 boundary. INTACK failed before reaching the target state and
the uninitialized x87 empty port crashed. It does not establish a product
crash. The corrected fixture retains these real initialization prerequisites.

DIV-883 changes one EOI port expression and its explanation, preserving
original masking, master acknowledgement, interrupt flag, ordering and other
branches. Rejected alternatives: an adapter that rewrites port writes,
synthetic clearing of ISR/device state, suppressed EOI or new retries. Those
would add replacement semantics instead of correcting the original typo.
No selected branch is restored or removed. Mirror raw EOL is preserved.

Reproducer: launch `python test/observation/verify-slave-bios-eoi.py
--build-root build/t478-s34/<fresh>` as an owned background process; it compiles
the fixture with the current selected graph flags and links the current
production archives, captures source/archive/EXE/map identity, and retains
the true fixture exit. Compile/link deadlines are 120 seconds each, execution
15 seconds. Full product build and acceptance remain required after the repair.

## Acceptance obligations at the first checkpoint

Rebuild and verify the correction, actual selected image/vector binding,
current BIOS dispatch and valid EMS lifecycle witnesses, related IRQ controls,
coherent package Full gates, exact worktree publication and local commit.
RTC-specific finding 27 remains S35. Historical source-only and invalid-EMS
tests do not substitute for these obligations.

## Candidate verification checkpoint

Coherent x86 worker/redirector/hook, WOW32 and all six x64 component targets
completed with exit zero (background PID 15000). Candidate focused runner
PID 15796, `build/t478-s34/slave-after`, passes 74 checks with zero failures.
All five slave lines complete two real PIC request/INTACK/D11 cycles: both
ISRs clear, IMR stays FFh and exactly one slave EOI callback occurs per cycle.
The baseline had 44 checks because each failed first retirement stops that
line's second cycle; assertions were not weakened between baseline and fix.
Controls remain unchanged. The frozen comparison is
`artifacts/mvdm-audit/shard-0/slave-bios-s34-progress.json`.
This checkpoint is not guest IRQ execution, Full acceptance or publication.

## Candidate guest and composition checkpoint

Candidate package `03d9792b-753e-4407-8a52-883ad9bc83e3` is staged under
`build/t478-s34/package-final`, not yet published. Background focused PID32456
passed real BIOS dispatch (836 cases) and EMS map/switch/alias/unmap/move/
exchange/window/free using the existing independent COM and original INT67
provider. The package binder PID29964 matches 52 functions, all 256 BIOS
slots and 30 EMS slots, including five D11 entries. Unlike the obsolete
observer, it records current source hashes and current graph/archive ownership
without asserting original byte equality or relying on an old dependency dump.

Related runner PID5328 rebuilt the existing native fixtures and passed 24
PIC-reset checks (including actual RTC/mouse callbacks), 86 keyboard/PIC checks,
real DOS IRQ0 control/reinitialization, and EMS AH56/BOP68 call/return stack/map
and negative cases. The EMS runner now selects the existing independent
call-return probe without reviving the historical SUBST runner. These outcomes
are recorded separately from S35's broader RTC question. Full is running in
that same PID; post-Full PID28432 waits for it before the five original guest
D11 vector/no-ISR routes. Neither pending check is currently claimed passed.

## Final acceptance and bounded closure

Full completed with all 16 selected gates passed in 379468 ms. The separately
queued real guest `SlaveBios` probe then passed all five original IVT routes
(INT72/73/74/76/77), checked stack restoration and the original no-ISR flag,
and exited zero. It neither replaces the IVT nor injects a synthetic EOI.
This guest test proves original vector -> BOP -> IRET wiring on the no-ISR
branch; the actual PIC-object fixture separately proves hardware request,
INTACK, ISR/mask and callback retirement. Their combination is not a claim
of hardware IRQ injection into a running guest. No guest bytes were changed.

| Finding-21 surface | Final disposition |
| --- | --- |
| BIOS/control/illegal dispatch | Existing original initializer and tables retained; 256 slot bindings, real 836-case error/no-op workload and D11 guest-vector witness pass. Empty original control tables remain explicit unsupported functions, not invented service implementations. |
| Valid EMS provider | Existing original INT67 allocation/map/switch/alias/unmap/move/exchange/window/free passes; existing AH56/BOP68 call-return verifies stack/map restoration and rejected subfunction/handle. No duplicate provider or missing wiring found; no generic rollback or arbitrary nested call guarantee added. |
| Slave unexpected interrupt | Source-proven original wrong-port bug corrected by DIV-883; before/after actual PIC evidence and unchanged sibling/AEOI/master controls establish the minimal repair. |
| Device retirement | Existing actual PIC-reset RTC/mouse callback and keyboard/PIC fixtures plus real DOS IRQ0 control/reinitialization pass. No rewrite of those earlier fixes; RTC-specific original BOP70/IRQ question retains S35. |
| CMOS helper and unsupported BASIC | Their selected functions remain image-bound. RTC helper effects remain S35; unsupported ROM BASIC is not missing a selected product BASIC interpreter. No optional profile revival or new BASIC capability. |

The binder records seven source files and 52 actual selected bodies, not a
whole-file behavior coverage percentage. The candidate set is resolved at
this contract scope; arbitrary malformed EMS inputs, all optional host
callbacks and exhaustive device schedules are not claimed newly covered.

`artifacts/mvdm-audit/shard-0/bios-s34-results.json` retains the baseline/fix
checkpoint, actual image/table results, source/run hashes, related checks,
16-gate result and published ten-image identity. Worktree assets/release and
its manifest exactly equal tested package 03d9792b-753e-4407-8a52-883ad9bc83e3;
prior package is backed up under build/t478-s34/release-before. Application
0.0.478/protocol67 unchanged. DOS4GW/NESticle stays explicitly excluded, not
passed. No O:/winnt writes or push. Final governance and diff review precede
the local S34 P1 commit; S35/finding27 is next.
