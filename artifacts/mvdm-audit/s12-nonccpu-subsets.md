# S12 — unselected non-CCPU host alternatives

DIV-505 replaces ten simple non-CCPU bodies in four product-selected MVDM
host C files while preserving original conditions and the selected CCPU paths.
All observed product compile commands for these files define CCPU; the
current-product `accessfn.c` exception is not touched. Each removed body
matches OpenNT, and the [ledger](s12-nonccpu-subsets.jsonl) records original
locations and hashes. The existing DIV-030 note was updated because its
non-CCPU duplicate definitions are now marked rather than retained. The
software CCPU V86 implementation and selected hardware-interrupt callback
interfaces are not replaced or removed.

Independent reconstruction checked the ten replacements and DIV-030 note in
four files against `bee08d9ea` with zero unrelated byte changes. Cached
NTVDM rebuild passed and all ten published outputs matched exactly or only
in link timestamps. No runtime regression or republish was needed for this
unchanged-code result. Mixed conditions, headers and other profiles remain
under audit; no guest was changed.
