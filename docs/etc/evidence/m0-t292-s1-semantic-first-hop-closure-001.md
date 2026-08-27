# M0 T292 S1 — semantic first-hop egress closure

> The function/file-level successor is
> [the first-hop closure record](m0-t292-s1-function-file-first-hop-closure-001.md).
> This evidence remains the authoritative source-expression and symbol
> classification input to that successor.

## Question

Has the selected MVDM source union been reduced from a conservative lexical
inventory to a real first-hop source-call inventory, with every actual
host-runtime call assigned one non-speculative owner/disposition before
package BFS begins?

## Inputs

- The provenance-selected 557 C/C++ files from the 1,689-file MVDM union.
- The raw lexical seed and its header-candidate index.
- The MVDM file disposition ledger, selected declaration carriers and original
  source/build ledgers.

## Procedure

1. Run `tools/governance/export-t292-s1-semantic-egress-ledger.mjs` against
   provenance-selected source paths, never against `src.old` or a current
   product build tree.
2. Mask comments, literals and preprocessor directives while retaining source
   locations.  Separately collect object-like/function-like macros and the
   historical SoftPC `IFN`/`IPT` declaration grammar.
3. Classify each code expression as a definition, macro/function-pointer
   expression, top-level declaration/initializer, selected-MVDM direct call,
   or external/unresolved direct call.
4. Retain only direct calls from file rows whose final disposition can enter a
   host runtime.  Calls confined to tool, guest, firmware or not-host-runtime
   source remain evidence, but cannot fabricate a host dependency.
5. Assign every surviving symbol one first-hop class and owner: public
   Win32/CRT, same-package MVDM/adapter, Native NT/loader facade, CSR/BaseSrv
   boundary, kernel-VDM monitor contract, Redirector, private Console, WOW,
   VDD, private GDI, or a named non-MVDM package frontier.

## Observations

- 89,506 code-shaped expressions were scanned.  Of those, 43,863 are macro
  uses, 26,834 are selected-MVDM direct calls, 4,786 are definitions, 1,603
  are top-level declarations/initializers and one remains syntactically
  unbalanced source evidence.  They are not external calls.
- The remaining 12,419 direct-call expressions reduce to 1,379 symbols.
  420 symbols occur only in source selected as non-host-runtime.
- Every remaining host-runtime symbol has a final first-hop disposition:
  858 public Win32/CRT bindings; 29 Native NT/loader contracts; 10 CSR/BaseSrv
  private-host boundaries; 12 Redirector/NetAPI private contracts; 10 private
  Console contracts; 9 private WOW contracts; 3 kernel-VDM monitor contracts;
  one VDD/input and one private-GDI contract; 13 macro/function-pointer
  contracts; 11 selected-MVDM/internal-adapter contracts; and two named
  non-MVDM OpenNT package-frontier symbols.
- The final symbol ledger contains zero `unresolved-stage-one-symbol` rows.
  Its per-symbol caller paths, selected caller disposition, upstream header
  evidence, first-hop owner and rationale are durable audit data.

## Interpretation

The earlier 5,988-spelling record is retained as a deliberately broad lexical
seed only.  It is not a first-hop closure: it includes comments, strings,
preprocessor forms, macro aliases and declarations.  This is the authoritative
source-expression and symbol-level result; the successor records the
function/file definition-candidate frontier without changing these symbols.

This closes **stage one only**.  A `non-mvdm-opennt-package-boundary`, CSR,
Console, Redirector, WOW or kernel-VDM row is a precisely identified BFS seed,
not permission to import or link its surrounding OpenNT product tree.  Stage
two must determine the exact reached definition and outgoing closure of each
such owner package.  Public Win32/CRT, selected MVDM helper and macro rows do
not generate a new OpenNT package node.

## Follow-up

Start breadth-first expansion with the two named non-MVDM package-frontier
symbols and the pre-existing build-proven Base VDM, BaseClient/NTDLL, Console,
USER/GDI and Redirector/NetAPI package nodes.  Preserve the same source-level
classification method for every subsequent package; do not fall back to a
header-name search as definition proof.
