# SOL dialog thread instruction samples

The integrated package completed three SOL Options interactions using the
test-only game-thread instruction sampler. The staged package identity and
raw observations are retained alongside this record. The NTVDM map and its
executable have matching link timestamps, and the executable hash equals the
staged manifest's NTVDM input hash. Package sealing changes the final image
hash. The currently available WOW32 binary does not match that manifest's
input hash, so no WOW32 symbol attribution from its current map is accepted.

Subsequent section-level verification narrows that limitation: the current
WOW32 image, the earlier unsealed input and the sampled candidate have the
same preferred image base, `.text` RVA, sizes and `.text` SHA-256; the full
images differ in resources. The current map has the recorded analysis hash
and was linked alongside its current image. The bounded [identity record](wow32-text-map-identity.json)
therefore supports nearest-symbol attribution of candidate code addresses,
not full-image identity or verified stacks. There are 20 WOW32 samples;
among them graph traversal appears six times, retained-generation lookup
three times, and graph-row indexing four times. Small sample counts and
sampling bias prevent treating these as timing shares or a root-cause proof.

The apparent WOW32 input mismatch also has a packaging explanation that must
not be mistaken for a relink: the integrated package's recorded input is the
previously sealed `projection-index-registered/package/system32/WOW32.DLL`,
whose own manifest identifies an earlier `projection-index-registered/inputs`
image. The cached current DLL, that prior input, and the integrated DLL share
the same `.text` hash and RVA. The package-identity reader finds no package
resource in the current cached DLL but finds identities in the prior input,
previous package and integrated package. Resource sealing is the documented
staging mechanism; this observation does not prove that the package-ID
resource is the only resource difference, nor that these full images are
interchangeable for product acceptance.

Across 336 samples, the open/close totals respectively include 50/23 in
win32u, 47/30 in ntdll, and nearest NTVDM symbols 17/10 for
`session_thread_current`, 12/15 for `phyR`, 11/8 for `qsort`, 8/7 for
`mvdm_softpc_physical_mapping_resolve`, and 8 open samples for
`copy_window_observation`. System-library samples have no function symbols.
The sampler periodically suspends the game thread; it changes timing and
samples instruction positions, not exclusive CPU time or validated stacks.
Nearest symbols can also misattribute folded or optimized code. These counts
corroborate repeated guest-memory and window-projection work as investigation
targets, but do not prove which operation causes the slowdown or establish
that a change is safe. Earlier outliers and Full acceptance remain open.
