# M0 T306 S1 P2 — Signature-seed package partition

## Question

Which original package boundaries are actually reached by the only 251
one-degree calls that already have a unique, declaration-reachable and
signature-confirmed original definition?

## Procedure

`tools/governance/export-t306-s1-signature-seed-packages.mjs` reads the
existing P20 signature gate only.  It preserves every physical call row and
uses the selected original definition's relative source path to apply the
finite package/stopping classification.  It neither reads a selected function
body nor chooses a runtime provider.

## Result

The generated [seed ledger](../operations/m0-t306-s1-signature-seed-package-ledger.tsv)
contains all 251 physical rows:

- 87 are original MVDM package bodies (`oemuni`, `dpmi32`, `suballoc`,
  `vdmredir` and related declarations). They are zero-degree rebase candidates,
  not external interface work.
- 76 enter `softpc.new`; they are explicitly retained machine-boundary seeds
  and cannot import the historical second executor.
- 54 enter `base/win32/client`, forming the source-backed BaseClient cohort.
- 16 enter `ds/netapi`, forming a bounded NetAPI candidate cohort.
- 15 are CRT operations and stop at the public MSVC CRT boundary unless a
  calling-convention audit disproves it.
- 2 enter COM/OLE source and require a public ABI comparison before source
  import.
- 1 enters `base/ntdll/csrtask.c`, a non-invasive CSR hard boundary.

## Interpretation

The signature-confirmed subset does not support a generic `adapter-common`
layer or a wholesale non-MVDM import. It instead identifies three concrete
next source-boundary cohorts: BaseClient, NetAPI, and the limited COM/OLE ABI
question. The first is already partially represented by the local Base VDM
form; T306 must determine whether importing more of the original BaseClient
body can remove that divergence. The second must stop at public RPC/Win32 or
a named hard boundary before it can become an `opennt-host` package. The final
two are leaves/boundaries, not product features.

## Follow-up

1. Rebase the 87 MVDM-internal rows through package-local binding evidence.
2. Inspect the BaseClient cohort breadth-first only to its CSR/public boundary.
3. Inspect the NetAPI cohort breadth-first only to its public RPC/Win32 or
   hard boundary.
4. Classify the remaining source-absent and non-signature-confirmed frontier
   separately; none may inherit this partition by spelling.
