# S12 — per-consumer unselected X86GFX header branches

DIV-512 marks eight original positive X86GFX branch bodies in six selected
MVDM headers. Current valid Ninja product dependencies were joined to their
actual compile commands: none of the observed selected consumers of these
headers define X86GFX. Thirty-two other MVDM headers have both X86GFX-on and
X86GFX-off consumers and are excluded from this batch. The selected `video.c`
source still defines X86GFX. Conditions, active alternatives, software CCPU
V86 and Video7 interfaces remain. Original body hashes and selected consumer
counts are in the [ledger](s12-x86gfx-header-subsets.jsonl).

Independent reconstruction verified six files and eight replacements against
`547a72cdf` with zero unrelated byte changes. Cached x86/x64 ten-component
build passed; after applying the released identity, all ten images matched
exactly or only in PE link timestamps. No runtime suite or republish was
repeated for unchanged code. No guest was changed. Mixed headers, compound
conditions and other profiles remain under audit.
