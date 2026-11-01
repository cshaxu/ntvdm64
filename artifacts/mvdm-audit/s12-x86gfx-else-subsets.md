# S12 — unselected X86GFX else branches

DIV-511 marks three original X86GFX alternatives in `nt_fulsc.c` and
`mouse_io.c`; observed product commands for these sources do not define
X86GFX, while the enabled `video.c` remains unchanged. The original conditions
and active software mouse/graphics alternatives remain. The
[ledger](s12-x86gfx-else-subsets.jsonl) records original body hashes and line
mapping.

Independent reconstruction verified both source files and three replacements
against `0a632265f`, including the recorded `#line` restore, with no
unexpected bytes. NTVDM rebuild passed; a changed diagnostic line value was
traced to the disabled else in `nt_fulsc.c` and its prior DIV-510 logical
line mapping, then restored. All ten published images matched exactly or
only in PE link timestamps. The original software text-cursor fixture passed
with mocked memory and geometry; it does not prove physical mouse behavior.
No guest was modified. No runtime suite or republish was needed for unchanged
code. Other X86GFX source/header contexts and compound selectors remain open.
