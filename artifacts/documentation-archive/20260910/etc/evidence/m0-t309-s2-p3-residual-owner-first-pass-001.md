# M0 T309 S2 P3 — residual owner first pass

## Question

Does the provider-cohort forced-link residual authorize new DEM/COMMAND
shims, or does it first select further original MVDM owners?

## Inputs and procedure

The x64 forced-link log from the formal T309 S2 graph was read as physical
`object caller -> symbol` records.  The result was compared with the existing
complete MVDM file disposition ledger and the original `mvdm-host` mirror;
bare symbol spelling was never used as an identity merge.

## Observations

- The audit contains 227 unique unresolved records.  Forty-nine are a stable
  `adapter-bochs -> bochs-core` mechanical lower boundary; 25 are MSVC C++
  runtime forms induced by the deliberate non-runnable DLL audit.
- The remaining 133 direct records originate in original providers: 90 DEM
  and 43 COMMAND.  The largest callers are `demdasd` (26), `demsrch` (15),
  `demmisc` (14), `demfile` (12), `cmdenv` (11), and `cmdmisc` (11).
- Reached targets such as `nt_floppy_*`, `HostFloppyReset`,
  `host_direct_access_error`, `GetPIFConfigFiles`, event-thread/console
  lifecycle forms, and standard-handle state are not invented gaps: their
  original bodies are respectively in `nt_rflop.c`, `nt_error.c`, `nt_pif.c`,
  `nt_event.c`, `nt_msscs.c`, `nt_lpt.c`, or `nt_hosts.c`.
- The complete final disposition ledger already distinguishes these files:
  `nt_error`, `nt_event`, `nt_hosts`, `nt_lpt`, `nt_msscs`, and `nt_pif` are
  adapter-backed original-source candidates; `nt_rflop` is explicitly
  not-host-runtime because its FDC/DMA/IRQ executor would duplicate Bochs.

## Interpretation

P3 must first select source groups, then bind their historical calls through
their existing named adapter families.  It may not manufacture a provider shim
for a symbol merely because the source body was not in P1's deliberately
narrow archive.  Conversely, the original `nt_rflop` device executor must not
be linked just because it owns a reached spelling; its contract is a
Bochs/firmware boundary.

The next P3 pass therefore partitions all 133 provider-origin records into:
original-source selection candidates, explicit Bochs/firmware exclusions,
public runtime leaves, and already-named adapter owner families.  It will keep
the physical caller identity on every row and will not enable a source body
until its full translation-unit boundary is audited.

## Complete physical disposition

`Export-T309S2ProviderResidualLedger.ps1` now exports all 133 x64 provider
records to `m0-t309-s2-p3-provider-residual-owner-ledger.tsv`.  Its unique
initial disposition counts are 40 Win32/public leaves, 26 CRT leaves, 23
original-source candidates, 20 Bochs/firmware boundaries, 19 existing adapter
forms, four debugger-product boundaries, and one WOW/second-executor boundary.
There are no unclassified rows.

The x86 forced link has the same 133 physical provider caller records.  Its
raw symbol spellings differ only in stdcall/import decoration, CRT spelling,
and x86 compiler SEH helper forms; it does not expose a different OpenNT
provider ownership set.

## Confidence and follow-up

High confidence for the counts and source locations: both come from the
formal link log and tracked final-disposition ledger.  Owner selection remains
open until the per-record ledger is complete.
