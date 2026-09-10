# M0 T276 S18: `ieuvddex` package disposition baseline

## Result

`ieuvddex` is not a VDD runtime provider. It is the historical `ntvdm.dll`
NTSD/WinDbg debugger extension: its command exports read another NTVDM
process's symbols and memory to print VDM_TIB, x86 context, LDT, virtual PIC
and DPMI trace state.

Consequently, all portable and x86 sources remain debugger-product inputs,
not runtime composition inputs. A future recovery belongs to
`adapter-debugger` and must preserve the extension-facing call and display
contract using a fixed-width copied session diagnostic snapshot. It must not
reach into the host process by historical address, expose a guest pointer,
install a generic VDD provider, or create another guest-memory mapping manager.

The six Alpha/MIPS/PPC help/build forms are retained as source evidence and
excluded from the x86/x64 profile.

## Ownership

- `adapter-debugger` owns a future same-shaped debugger extension facade.
- The session owns any diagnostic snapshot publication and mapping-manager
  lifetime; the extension has no private guest-memory mapper.
- `adapter-softpc -> adapter-bochs` remains the only path to live machine
  mechanics; debug data is copied out rather than granting direct access.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S18IeuvddexDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S18IeuvddexDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```
