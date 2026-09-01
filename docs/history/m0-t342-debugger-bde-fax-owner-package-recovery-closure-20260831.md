# M0 T342 — Debugger / BDE / FAX owner-package recovery closure

## Closed result

T342 establishes that the one original debugger form required by normal MVDM
startup is `DBGInit`. Its same-shaped, bounded adapter is selected and links
in both formal CPU40 build rows. The source/ABI/failure evidence is recorded
in the [S1 ledger](../etc/operations/m0-t342-s1-debugger-bde-fax-owner-contract-ledger.tsv)
and [S2 verification](../etc/evidence/m0-t342-s2-debugger-startup-formal-closure-001.md).

## Explicit non-runtime dispositions

- `dbg.c` debug-event transport: private NT4 debugger boundary.
- BDE, VDMDBG and VDMEXTS: original debugger tools/extensions.
- WOW debugger notifications: future combined WOW/debugger lifecycle owner.
- WOWFAX and WOWFAXUI: printer/GDI driver and configuration products.

None was deleted, stubbed as a false success, or added to the product link.
Future work must admit the complete owner package rather than reopening this
startup probe by trace.
