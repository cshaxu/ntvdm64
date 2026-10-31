# S12 — modified non-PROD debug branches

DIV-502 replaces two disabled non-PROD branches that had previous project
changes. `c_main.c` contained a non-PROD debug-event check using the adapted
CCPU event state; `dasm.c` contained the debug disassembler after DIV-493
hardware-monitor/CPU30 removal. Both are enclosed by original `#ifndef PROD`
and product source commands define PROD. The active CCPU event handling,
software V86 and production paths remain unchanged. The original outer
conditionals remain, with a numbered exclusion comment.

The [ledger](s12-nonprod-modified-branches.json) records the Git baseline,
old body hashes and prior changes. Cached NTVDM rebuild passed; after
applying published identity, all ten product images matched the published
set exactly or only in PE link timestamps. No guest was changed and no
runtime regression or republish was needed for unchanged code. Other
condition expressions and unsupported profiles remain under audit.
