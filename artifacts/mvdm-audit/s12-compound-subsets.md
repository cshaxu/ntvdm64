# S12 — fixed compound condition source branches

DIV-515 marks 15 original branch bodies in seven selected MVDM host C files
whose compound conditions evaluate to a fixed excluded branch in every
observed product command for that source. Expressions are limited to boolean
combinations of the verified `CCPU`, `CPU_40_STYLE`, `PROD`, `PIG`, `MONITOR`
and `X86GFX` selectors; unknown symbols, arithmetic, mixed results,
project-marked bodies and source differences are not auto-resolved. Examples
include `CCPU && !CPU_40_STYLE` and `CPU_40_STYLE && !CCPU`, neither of which
is selected by their actual source commands. Original conditional shells,
active alternatives, selected CCPU/V86 and prior divergence markers remain.
The [ledger](s12-compound-subsets.jsonl) records each expression, actual
per-source command macro values, original body hash and baseline.

Independent reconstruction checked seven files and 15 replacements against
`b03bd6cde` with no unexpected byte changes. Cached NTVDM rebuild passed;
after aligning published identity all ten images matched the released set
exactly or only in PE link timestamps. No runtime suite or republish was
required for unchanged code. No guest was changed. Headers with mixed
consumers, unsupported profiles and other condition expressions remain open.
