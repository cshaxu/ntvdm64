# M0 T276 S17: `v86` package disposition baseline

## Result

The 35-path `v86` root is three historical products, not an alternate CPU
engine:

- `monitor/i386` is the NT kernel-VDM monitor.  It consumes `VDM_TIB`,
  `NtVdmControl`, CCPU/SAS/SoftPC callbacks and direct historical VDM memory
  pointers.
- `scaffold/i386` and its resources form the old standalone `ntvdm.exe`
  startup shell.  It initializes SoftPC, DEM, XMS and COMMAND itself, which
  would duplicate the modern app's composition responsibility.
- `util/quit.asm` is an independently built 16-bit `BOP_UNSIMULATE` tool, not
  a host runtime input.

The original monitor retains crucial interface evidence: copied CPU-frame
access, BOP/interrupt/port dispatch, VDM event ordering and guest-memory
address forms.  A later `adapter-vdm-monitor` recovery can preserve those
forms only through the session-owned guest-memory mapping manager and
`adapter-softpc -> adapter-bochs`; it cannot recreate `NtVdmControl`, expose
raw host pointers or install a second V86 execution engine.

## Ownership

- `adapter-vdm-monitor` owns a future same-shaped monitor facade.
- The session owns the unique guest-memory mapping manager and lease lifetime.
- `adapter-softpc -> adapter-bochs` owns the source-shaped machine transition.
- `app` owns modern product composition and is the only caller of
  `adapter-bochs`.
- `opennt-mvdm-tools` owns the retained 16-bit tool input.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S17V86Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S17V86Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```
