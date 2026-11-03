# S13: selected headers and further original line mappings

Three byte-identical original headers were audited in their entirety. Their
selected single conditional groups were witnessed in actual x86 preprocessing;
see [header evidence](s13-r657-selected-headers.json). This is not a claim
that every supported context has been enumerated.

Ten other C/H files received original diagnostic line mappings for 51
previously excluded arms; see [guard evidence](s13-r658-guard-line-audit.json).
Their other conditional groups remain pending. No selected logic was replaced.

Cached x86, x86-extra and x64 builds passed. The x86 retained-guard control
passed 181 checks and MSVC line controls passed for all 51 groups. Seven of
the ten product images are byte-identical and three differ solely in parsed
PE timestamps; [comparison](s13-r658-build-image-compare.json). The prior Full
17/17 evidence is reused per owner instruction, not rerun. The complete ten
images were published and hash-verified in both destinations. Guest images
were neither modified nor rebuilt.

The [counts](s13-r658-counts.json) report present files separately from
deletions. Remaining file audits and full context enumeration remain open.
