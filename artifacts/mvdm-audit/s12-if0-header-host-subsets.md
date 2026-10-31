# S12 — original literal-zero header and host subsets

Following MVDM-HOST-DIV-496 and OPENNT-HOST-128, this batch replaces the
original disabled bodies of 13 literal `#if 0` branches in eight host files
with registered exclusion comments, preserving directives and active
alternatives. Six branches are in headers present in the observed dependency
union; seven are in `nt_com.c` / `nt_fulsc.c`, where later `__LINE__` use
requires particular output comparison. The original bodies were matched to
local OpenNT and independently reconstructed against commit `64f0be779`:
eight files, 13 branches, zero unexpected source-byte changes. The
[per-branch ledger](s12-if0-header-host-subsets.json) records exact paths,
original line ranges and hashes. Divergences MVDM-HOST-DIV-497 and
OPENNT-HOST-129 carry the owner-authorized deletion rule.

Cached x86/x64 ten-component rebuild passed. Initial NTVDM comparison found line-number constants changed in `nt_com.c` and `nt_fulsc.c`. Registered `#line` restorations preserve those constants; the latter also requires restoration after the enclosing inactive X86GFX block. After rebuilding, all ten images matched the published package exactly or only in link timestamps. No repeat runtime suite or republish was required for timestamp-only differences. Other
conditional paths remain in scope, including selectors other than literal
zero and files not selected by the ten product graphs. No guest was changed.
