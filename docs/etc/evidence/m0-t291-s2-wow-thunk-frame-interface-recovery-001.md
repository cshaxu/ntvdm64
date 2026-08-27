# M0 T291 S2 — WOW thunk/frame interface recovery design

## Question

Can the first original WOW32 thunk/frame subdomain use an existing adapter
family without turning historical pointer aliases or monitor state into a
modern, unbounded product shell?

## Inputs

- The twelve byte-exact-or-registered original WOW32 source bodies selected by
  T291/S1.
- Original `wow32.h`, `wcall16.h` and `mvdm-support/inc/wow.h` declarations.
- Current `adapter-mvdm-host-out/{softpc,monitor,wow}` and session contracts.

## Procedure

Run the occurrence exporter:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools\governance\Export-T291WowThunkFrameInterfaceAudit.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

It enumerates every non-comment selected-body occurrence of the reached
historical interface tokens, attaches its original call form and records a
single source/ABI/failure disposition.  The companion recovery decision was
then reviewed against the existing SoftPC and monitor adapter contracts.

## Observations

- The [occurrence ledger](../operations/m0-t291-s2-wow-thunk-frame-interface-occurrence-ledger.tsv)
  covers every reached selected-body occurrence; the
  [family ledger](../operations/m0-t291-s2-wow-thunk-frame-interface-family-ledger.tsv)
  gives each source token one owner and future binding result. The
  [source-coverage ledger](../operations/m0-t291-s2-wow-thunk-frame-source-coverage-ledger.tsv)
  explicitly records all twelve selected bodies, including those without an
  S2-scope boundary call.
- Existing `mvdm_redirector_pointer_scope` deliberately cannot serve WOW:
  it requires fixed predeclared spans and does not model dynamic WOW structure
  converter lifetimes.
- Existing monitor TLS provides selected DPMI `VDM_TIB` fields only.  It does
  not create a historical WOW task/stack projection, CCPU executor or CSRSS
  broker.
- The original `CallBack16` body has a strict frame-write, stack-switch,
  execute, result-read and stack-restore sequence.  That sequence is the
  required future source-shaped transaction boundary.

## Interpretation

The existing adapter families are sufficient in ownership terms: no tenth
generic adapter or new mapping manager is warranted.  They are not sufficient
as already implemented interfaces.  S3 must recover the complete bounded
WOW-pointer/frame/callback transaction as one interface group before enabling
any of the twelve original bodies.  Private PEB lock, fast-monitor and
whole-memory-alias paths remain explicit source-defined fallback/unavailable
branches until their named owner packages are admitted.

## Follow-up

Admit only the S3 whole-interface implementation group described by the S2
decision.  Do not implement an individual WOW API or BOP from a trace.
