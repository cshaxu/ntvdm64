# External audit obligations: scope is not dependency

The frozen WOW external inventory has 42,963 edition-specific IDs outside
the 4,684-path MVDM union. The current DOS16 catalog has 3,520 entries, all
within MVDM; it introduces no second outside-MVDM population at this snapshot.
These are file/edition identities, not function counts or restoration rates.

The [per-ID triage](external-obligation-triage.csv) joins every frozen ID to
its current audit record and pinned original Git index. The
[summary](external-obligation-triage-summary.json) records exact inputs and
limits. `tools/audit/TriageMvdmExternalObligations.py --output
<fresh-build-directory>` reproduces this read-only join. It verifies both
original repository revisions and all 42,963 Git blobs against their actual
indexes. Only nine reviewed recipe contents are freshly SHA-256 checked;
Git blob identity must not be presented as a current working-content hash.

The original extractor explicitly enumerates configured provider directories;
membership is not an effective dependency or runtime admission:

| Tier | Rows | Actual obligation |
| --- | ---: | --- |
| Device context, expansion gated on a specific dependency | 36,736 | Preserve identities and investigate concrete caller/data/format dependencies before selecting original provider source. Not blanket exclusion or proof that modern providers preserve every old device behavior. |
| Other host-provider envelope, unresolved | 6,134 | Establish exact caller/provider, selected slice and outgoing boundary; directory membership alone cannot authorize importing a full NT4 subsystem. |
| Explicitly named source-audit boundary/context | 93 | Prioritize its precise claim and original chain; records include direct boundaries, downstream dependencies, recipes, clients and binary evidence. Not all are direct current MVDM calls. |

All tiers retain their unproved applicability explicitly. Nine particular
native-provider build descriptors have evidence supporting file-role exclusion;
the other 42,954 rows remain pending-specific-contract-review. This triage is
not a way to close those rows without investigating needed MVDM capabilities.
It prevents confusing a complete source-directory census with a requirement
to port every NT4 printer, display driver, kernel or USER/GDI server.

The supporting reviews are [inventory obligations](shard-2/external-inventory-obligation-review.md),
[printer owners](shard-1/external-printer-provider-review.md),
[six printer recipes](shard-1/external-printer-recipe-examples.csv), and
[display/layout/font owners](shard-3/external-device-provider-review.md).
Unresolved cases include PSCRIPT/RASDD compatibility, font or keyboard mapping
data and shared declarations. The original WOW conversion and guest-facing
algorithms remain required independently of the modern host's actual device
resource ownership. A successful PDF or dialog test proves neither every
historical printer nor every resource/escape.

The loader frontier also produced a concrete
[BOOLEAN return ABI finding](dll-entry-boolean-return.md), with an actual-object
controlled witness and a one-declaration test candidate. It is assigned to
S11; no runtime repair or publication occurs in this source-audit checkpoint.
The external-provider decisions alone change no MVDM classification, and S10
is not closed.

The later live-source sweep also registers two concurrent, uncommitted video
source differences as partial; see [candidate observation](live-candidate-source-observation.md).
This is not candidate acceptance or inclusion of those source changes in the
delivery. The current MVDM totals are 52 full, 226 partial, 1,466 excluded and
2,940 pending. The 42,963 external IDs remain a separate source-edition census.

The [checkpoint verifier](external-obligation-checkpoint.py),
[result](external-obligation-checkpoint-result.json) and
[background record](external-obligation-checkpoint-runner.json) crosscheck
the complete external ID join and recorded input hashes, the controlled ABI
witness and published bridge correspondence, all 4,684 MVDM partitions and
the two live-difference classifications. Documentation governance and
whitespace checks passed. This is not a whole-provider, real DLL-failure or
full-product regression result.

A read-only freshness check of the seven recorded triage inputs found each
SHA-256 unchanged, including the current external DOS16 source catalog and
WOW32 source-coverage, seam registry and scope. The current WOW32 frontier
and evidence-map hashes also match the individual recorded hashes in the
[inventory review](shard-2/external-inventory-obligation-review.md).
[Input results](external-input-freshness-current.json) bind this check. It
confirms the recorded input identities only; it does not prove that the
pending provider obligations or runtime capabilities have been resolved.
