# M0 T342 S1 — Debugger / BDE / FAX owner-package admission

## Result

Only the original `DBGInit(argc, argv)` startup probe is part of the current
formal product closure. It is correctly represented by the existing narrow,
same-shaped debugger adapter. All other inspected debugger/BDE/FAX source
packages are either original debug tools/extensions or Windows printer/GDI
products, not missing host-runtime providers.

## Source facts

- `nt_msscs.c::scs_init` is the direct original caller of `DBGInit`.
- `dbg.c::DBGInit` only probes `ProcessDebugPort`, stores a boolean and returns
  success. The rest of `dbg.c` produces `STATUS_VDM_EVENT` exception payloads
  and consumes the private NT4 VDM debugger transport.
- `wkman.c` and `wow32.c` call the three original `DBGNotify*` forms, but they
  require the separate WOW task/debug lifecycle; they do not make a generic
  debugger dispatcher a current startup dependency.
- Original build files classify BDE and VDMEXTS as NTSD extensions, VDMDBG as
  a debugger DLL, WOWFAX as an NTGDI printer driver, and WOWFAXUI as its
  printing configuration UI.
- Both current formal maps resolve `DBGInit` from `debugger-bindings:dbg_init.obj`;
  none links a BDE, VDMDBG, VDMEXTS, WOWFAX or WOWFAXUI object.

## Disposition

The adapter is retained because it removes exactly one unavailable private
NT4 query while keeping the original ABI and result direction. Replacing it
with full `dbg.c` would import a larger private debugger transport without a
selected debugger client. No new adapter or replacement behavior is selected.

The accompanying ledger gives every source form a unique owner and transfer.
It is an admission result, not an assertion that debugging, WOW task
notifications or fax printing works.
