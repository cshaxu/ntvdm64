# M0 T257 S3 — DOSX/DPMI Startup and Descriptor-Table Closure

## Scope

S3 admitted only the selected DOSX/DPMI startup/table source subset. It did
not admit an active `53:xx` BOP service, protected interrupt/fault delivery,
IRET composition, XMEM, or PM-to-DOS translation.

## Exit-criterion audit

| Required fact | Evidence | Result |
| --- | --- | --- |
| Exact-copy source set is identified and all divergences are named | S3 source mirror manifest; startup source evidence 002; descriptor source evidence 005 | Pass |
| Startup/table source formally links through named seams | `dpmi32.c`, `dpmiselr.c`, the three named seams and formal-r6 | Pass |
| Historical process-LDT imports are rejected | S3 descriptor boundary gate; `NtSetLdtEntries`/`ProcessLdtInformation` absent from composed seam | Pass |
| No duplicate descriptor mechanism exists | fixture 004 proves native `LGDT → LLDT → DS`; source seam carries only temporary copied entries and checked guest-table write | Pass |
| No active leaf BOP route was introduced | `dpmi32.c` table remains source-shaped deferred; active ingress is not a formal input | Pass |

## Verification

- `build/M0-T257-S3/formal-r6/`: **450/450** MSVC x64 `/MT`, CPU5/P-MMX
  Ninja targets built.
- `t257-s3-dpmi-startup-session-fixture.exe`,
  `t257-s3-native-guest-ldt-fixture.exe`, and
  `t257-s3-dpmi-descriptor-source-fixture.exe` each exited `0`.
- `Test-T257S3DpmiDescriptorBoundary.ps1` and
  `Test-T237S4MachineSemanticBoundary.ps1` passed.

## Transfer

S4 receives the whole protected interrupt/fault/IRET source package:
`dpmiint.c`, `dpmiint.h`, `stack.c`, `dpmimscr.c`, the corresponding DOSX
`dxintr.asm` framing contract, and historical hook imports. Native Bochs
continues to own architectural exceptions and IRET; S4 may recover only
OpenNT policy/order through a bounded bx-vdm frame-session boundary.
