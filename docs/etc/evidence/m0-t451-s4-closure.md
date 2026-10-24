# M0 T451 S4 — USER object acceptance and handoff closure

## Delivered scope

S4 closes the accepted consumer/handoff scope of T451's one copied USER view.
It adds no production provider, private USER table, guest change, or new
cross-component ABI.  The authoritative detailed audit is
[object handoff audit](m0-t451-s4-object-handoff-audit.md).

The retained ownership chain is:

```
modern USER facts -> S2/S3 page-domain copied view -> original WOW32/USER client
```

Host HWND/public USER state remain host authority; the page domain owns the
one original-layout projection.  The selected original consumers use the
redirect only for the original private PWW/PWC slots that modern USER does not
expose.  Ordinary Window/Class Long indices remain native USER calls.

## Consumer accounting

The selected provider compiles 82 WOW32 translation units through the common
private redirect.  The 15 actual original Window/Class Long consumer units are
accounted for in the audit: dispatch/lookup, subclass/hook, dialog/common
dialog, DDE/structure/fax and diagnostic paths.  No admitted client body has a
second private PWW/PWC reader or an independent copied-view owner.

## Verification

| Boundary | Evidence | Result |
| --- | --- | --- |
| Direct data, callbacks and object lifetime | `build/t451-s3/desktop-current/matrix/summary.json` | 19 private-desktop rows, `failed: 0`; current candidate/release/install artifact hashes are unchanged since that run. |
| Immutable WINMINE interaction | `build/t451-s4/winmine-r5/result.txt` | Flag, reset, open cell, `WM_CLOSE` and normal Run16 completion pass. |
| SOL visible startup frontier | `build/t451-s4/frontiers-logs/t451-s4-sol-r1-sol-windows.txt` | One staged `ntvdm.exe`, visible `Solitaire` and `Stat` in all four samples. |
| WRITE visible startup frontier | `build/t451-s4/frontiers-logs/t451-s4-write-r2-write-windows.txt` plus `verify-wow-write-frontier.ps1` | One staged `ntvdm.exe`; `MSWRITE_MENU`, `MSWRITE_DOC`, `MSWRITE_PAGEINFO` in all four samples; verifier passes. |
| Package identity | candidate, `assets/release`, `O:\\winnt\\system32` SHA-256 | `WOW32.DLL` is identical (`475275C6B1D9F4976CC5371860D0EF19F072D07CE1…`). |

SOL and WRITE assertions are deliberately startup/frontier evidence, not a
claim of complete application interaction or normal exit.  WINMINE is the
selected interaction-level guest regression.

## Boundary retained from S3

Modern public host USER cannot distinguish every original NT4 private
`WFDESTROYED` destroy transaction from a hand-sent `WM_NCDESTROY`.  The S3
closure records this finite host boundary.  No synthetic replacement was
introduced; the reachable selected destruction chain is covered by the
accepted matrix.

## Delivery and disposition

The latest S4 source/evidence checkpoint is published on `origin/main`.
S4 changes only evidence/governance and uses the already hash-verified product
package; it does not require a binary republish.  T451 has no unadmitted S5.
This closes S4's finite scope and leaves the T-level closure pending owner
acceptance, rather than silently closing the T.
