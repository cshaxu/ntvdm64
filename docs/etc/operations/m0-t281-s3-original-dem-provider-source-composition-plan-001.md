# M0 T281 S3 — original DEM provider source composition plan

## Purpose

Treat the remaining original DEM provider translation units as one owner
package. Establish their x86/x64 source-composition disposition before any
provider is linked or called. This is package recovery, not a trace-derived
service-by-service implementation loop.

## In-scope original units

`demdasd.c`, `demdir.c`, `demerror.c`, `demfcb.c`, `demfile.c`, `demgset.c`,
`demhndl.c`, `demioctl.c`, `demlabel.c`, `demlock.c`, `demmisc.c`, and
`demsrch.c` from `dos/dem`.

## Required method

1. Use the existing T280 source/ABI ledgers and the closed S2 declaration form
   as the only initial composition surface.
2. Run an x86/x64 compile survey of all twelve exact or registered-divergence
   units, without linking or invoking them.
3. Give each compiler block one source-level classification: direct existing
   binding, existing adapter boundary, missing original declaration carrier,
   or future owner package. Do not write a new provider or generic shim.
4. Only after that complete survey, create a static archive containing the
   units that are compile-safe under existing bindings; explicitly list every
   excluded body and reason.

## Exclusions

- `DemDispatch` or any `50:xx` invocation;
- executable/link/runtime targets, a guest trace, or host filesystem action;
- a new adapter ABI, raw guest pointer, Bochs include, or mirror-body edit;
- resolving a single BOP leaf ahead of its owner-package disposition.

## Exit criteria

- All twelve units have an x86/x64 reproducible outcome and source hash form.
- Every new failure has one owner and migration disposition in the S3 ledger.
- A formal archive, if emitted, has no provider invocation or runtime route.
- Evidence records whether the package can advance unchanged, needs an
  existing binding correction, or stops for a separately admitted adapter.
