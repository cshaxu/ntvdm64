# M0 T289 S5 — Stack, interrupt and fault composition closure

## Question

Has the admitted DPMI stack/interrupt/fault source group reached a truthful
closure point without enabling a partial `53:xx` route?

## Inputs and procedure

- Reviewed P1--P6 evidence and the current stack/interrupt disposition
  ledger.
- Rebuilt the selected original `data.c`, `register.c`, `stack.c`,
  `dpmiint.c`, `dpmimscr.c`, `modesw.c` and `savestat.c` source group on both
  formal MSVC `/MT` graphs:

  ```powershell
  powershell.exe -ExecutionPolicy Bypass -File \
    tools/build/New-T289S5DpmiInterruptAuditNinja.ps1 -Architecture x64
  ninja -C build/M0-T289/S5/audit-x64

  powershell.exe -ExecutionPolicy Bypass -File \
    tools/build/New-T289S5DpmiInterruptAuditNinja.ps1 -Architecture x86
  ninja -C build/M0-T289/S5/audit-x86
  ```

## Closure facts

- Original registration bodies, locked PM-stack switching, PM-stack opaque
  identity, copied real-mode CPU transition and the two real-mode stack/IVT
  entrypoints have source-shaped x86/x64 local proof.
- The remaining original `DpmiSwitchToDosxStack` / `DpmiSwitchFromDosxStack`
  body requires the monitor-owned guest-visible VDM-TIB/DOSX projection.
- Protected dispatch, IRET and fault paths require source LDT descriptor
  publication plus checked protected stack-frame composition. The selected
  host-LDT terminal result remains the original unavailable direction; no
  second LDT or CPU executor was introduced.
- Historical CCPU, `NtVdmControl`, `VdmInstall*Handler` and CSRSS branches
  remain excluded under their source profile and have not been replaced.
- No `53:xx` ingress, selector recognition or BOP route has been enabled.

## Interpretation

S5 is locally regression-closed for every independently composable stack,
interrupt and fault mechanic. Its deliberately unavailable branches have
named monitor/descriptor owners rather than silent fallbacks. The following
S6 owns the family-wide `53:00..18` local disposition matrix; it must not
reinterpret this closure as protected-mode runtime continuity.
