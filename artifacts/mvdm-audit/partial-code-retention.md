# Partial source code-retention metric

`partial-code-retention.csv` covers all 205 paths currently classified
partial in the frozen MVDM union, including the eight raw-format-only
differences. Run `python tools/audit/EstimateOriginalCodeRetention.py` from
the repository root. The tool verifies both original OpenNT and current raw
SHA-256 identities against `frozen-file-reconciliation.csv` before writing.

The measure is matched original nonblank code lines divided by the count of
original nonblank code lines. A lexical pass removes C comments and assembly
semicolon comments while retaining quoted text and preprocessor lines; each
line is trimmed. Python `difflib.SequenceMatcher` with autojunk enabled gives
a deterministic approximate match, not a compiler or semantic analysis.

This is **textual retention**, not the proportion of functional capability
restored. An inserted conditional or early return can change behavior while
all original code lines remain present; a rewritten but equivalent function
can lower the score. Conditional selection, platform edition, added adapters,
state ownership, guest/host behavior, error and lifecycle paths are not
measured. Fifty files happen to score 100% on this text measure, but every
one remains partial because its raw mirror differs. The functional estimate
fields remain explicitly unmeasured until source and behavior evidence support
them; this CSV is an independently reproducible quantitative input, not a
claim that the task's functional audit is complete.
