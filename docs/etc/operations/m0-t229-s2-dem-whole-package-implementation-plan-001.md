# M0 T229 S2 — DEM Whole-Package Implementation Plan

## Binding basis

S1's 73-row ledger is the complete population.  Every change must preserve the
original `DemDispatch` table slot, source owner, register/checked-memory ABI,
dispatch order and failure/continuation rule.  Existing typed routes are
implementation inputs, not completion evidence.

## Continuous implementation groups

1. **File, handle, FCB and search (29).** Recover original path, DTA/SRCHBUF,
   handle and FCB contracts through checked guest copies and opaque
   session-owned tokens.  Direct uses admitted ordinary Win32 capabilities;
   Readonly returns the original/source-derived mutation failure before host
   mutation.  Pipe EOF routes retain their original Redirector-deferred
   outcome.
2. **GSET, drive, CWD and volume (15).** Replace snapshot/synthetic fallbacks
   only with source-shaped host capability contracts; preserve drive, DPB,
   date/time, directory and volume layouts.  Direct and Readonly share the
   namespace view; only mutation disposition differs.
3. **Misc, lifecycle, console and WOW (15).** Recover only the portions owned
   by DEM.  VDD, engine terminal and WOW dependencies receive the original
   explicit unavailable/deferred outcome until their own later packages;
   never fake success.
4. **Error/lock (3), DASD/IOCTL (3), original no-op (7), FastRead (1).**
   Preserve packet/register-specific refusal or no-op contracts, including
   CF-clear for original `demNotYetImplemented`; decide FastRead as a package
   contract, never an accidental compatibility fallback.
5. **Convergence.** Remove or migrate every bypass/fallback identified by S1
   only after equivalent package-session coverage exists.  Run all-73 Direct
   and Readonly regressions; Overlay is negative ABI-extension coverage only.

## Prohibited shortcuts

No synthetic DOS kernel/MCB/PSP behavior; no adapter-owned CPU/device
semantics; no guest-image change; no ambient host-state fallback; no trace-led
single-service admission; no use of Overlay or Virtual to evade a Direct or
Readonly failure contract.

## Completion evidence

Each group receives a source/ABI/failure record and focused regression.
A single whole-family matrix then proves 73-cardinality, Direct/Readonly
outcomes, original no-op and invalid-service boundaries, and absence of
unbound legacy bypasses.  Only then is one bounded native guest observation
permitted as integration evidence.
