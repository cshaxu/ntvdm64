# M0 T277 S5 — SoftPC host-control source-form family audit

S5 completes the source-form review of all 197 future-host correlations whose
callers reside under original `softpc.new/host/src`.  The result is a
nine-family, implementation-disabled ledger—not an enabled host build.

- **39 CPU/SAS/PIC façade rows** retain original calls from `nt_aorc.c`,
  `nt_cpu.c` and `sim32.c`, but route exclusively through
  `adapter-softpc -> adapter-bochs`.  CCPU, CVIDC, original PIC, video and
  device execution bodies remain Bochs-replaced; this explicitly prevents a
  second executor.
- **Nine BOP-control rows** retain their `nt_bop.c` source shapes and enter
  only through `adapter-bop`.  `MS_bop_5` has an explicit `adapter-vdd`
  successor; no selector becomes known to Bochs.
- **27 session stop/event rows** retain `nt_event.c`/`nt_msscs.c` ordering,
  while `TerminateVDM` becomes session controlled stop instead of historical
  process termination.  Private VDD/CSRSS branches are named successors, not
  silently emulated.
- **36 serial endpoint rows** keep `nt_com.c`/`nt_ntfun.c` signatures and
  call ordering.  Host endpoint identity belongs to the single session
  `host_resource` mapping instance; historical SoftPC serial execution does
  not re-enter the product.
- **33 mouse/display/console rows** preserve original host provider shapes
  through public Win32/session state.  Old fullscreen and WOW UI branches are
  explicitly deferred to their monitor/WOW owners.
- **16 host-device/resource rows** retain original disk, printer, error,
  detection, file and timing host forms.  They use public host capability only
  behind `adapter-win32`; direct device policy is not recreated in a caller.
- **10 monitor/debugger/VDD rows**, **25 CRT/resource rows**, and **two
  COMMAND package entry rows** respectively retain their original package or
  public-runtime shapes, with named adapters for only the historical private
  product boundaries.

Every one of the 197 rows records the original provider form, sole future
owner, interface shape, mapping rule and failure disposition.  The lexical
candidate ledger remains supporting evidence only; the S5 family ledger is
the actual reviewed disposition.

## Reproducibility

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T277S5SoftpcHostControlDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S5SoftpcHostControlDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```
