# T230 S12--S14 shared `DemDispatch` local matrix

## Result

The complete 73-slot Direct DEM local matrix now enters the directly imported
OpenNT dispatcher through `bx_ntvdm_demdisp_invoke`; it no longer uses an
owner-specific shim as its top-level fixture route.  This is local
source/ABI/failure evidence only.  It is not native-engine composition and
does not make retained `bop-v1` runtime code unreachable.

## Route

`typed DEM call -> bx_ntvdm_demdisp_invoke -> original DemDispatch ->
original apfnSVC table -> imported OpenNT owner body -> checked result`

The bridge remains selector-blind: BOP decode stays outside the imported DEM
surface, while the original `demdisp.c` table retains service ownership and
ordinary-return behavior.

## Package witnesses

| Completion group | Formal fixture | Source owners exercised through `DemDispatch` |
| --- | --- | --- |
| S12 | S2--S5 | `demhndl.c`, `demfile.c`, `demlabel.c`, `demdir.c`, `demsrch.c`, `demfcb.c` |
| S13 | S6--S7 | `demgset.c`, `demdasd.c`, `demioctl.c` |
| S14 | S8--S10 | `demerror.c`, `demlock.c`, `demmisc.c`, dispatcher ordinary/no-op bodies |

The S10 fixture executes `50:1D` and every original `demNotYetImplemented`
identity (`1F,24,26,28,2B,40,42,43`), checking the original clear-CF return.
The S8 retry case restores a saved ordinary-return service identity and lets
the imported `demRetry` re-enter the original dispatch table; the prior
shim-local `ERROR_CALL_NOT_IMPLEMENTED` expectation was removed because it
was not the original complete-table behavior.

## Reproduction

Generate `build/M0-T230-S12/dispatcher-ns-r1` using
`tools/build/New-T225S7FullNinjaGraph.ps1`, then build and run the nine
`t230-s2` through `t230-s10` fixture targets.  The formal MSVC x64 `/MT`
run completed with zero exits for all nine fixture executables.

## Remaining T230 work

S15 must remove every formal/native `50:00..48` route through `bop-v1` DEM
composition.  S16 must bind this Direct session to mantle checked-memory
transport and record the bounded native observation.  Neither condition is
claimed here.
