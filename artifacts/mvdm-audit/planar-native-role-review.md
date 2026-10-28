# S10 original planar owner reconciliation

Base: `bea69f8b4`. This checkpoint resolves exactly three pending physical
source roles to partial. It does not recover or accept every rendering mode.

## Source and actual execution ownership

The [image observer](planar-image-observer.py) compares current files byte for
byte with original OpenNT and joins each to the frozen DOS16 catalog and both
OpenNT/OpenNT-4.5 WOW32 catalog rows. Their exact edition hashes and catalog
identifiers are retained in [observations](planar-image-observations.json).
The original NT `host/src/sources` recipe is unchanged. Each selected object
has a valid retained compiler dependency block and an actual archive edge.

All 40 mapped function bodies match the held candidate image outside COFF
relocation words: nt_cga 14, nt_ega 23, nt_munge 3. Cached production and held
candidate executable sections also match; different package UUID resources
are not treated as code differences. This is code identity, not branch or
guest-behavior coverage.

Three original colour PAINTFUNCS vectors in nt_graph have 11 entries each.
All 33 actual image pointer values resolve to their original selected owners;
the CGA medium and EGA medium/high slots select the expected scale-specific
functions. Six actual REL32 call targets from EGA medium/high painters select
the three original munge implementations. nt_graph's existing mirror changes
are neither rewritten nor promoted to full by this evidence.

| File | Disposition and authority | Remaining finite S13 work |
| --- | --- | --- |
| nt_cga.c | Partial. Original colour LUT conversion writes the host-owned DIB under its kernel mutex, then invalidates its rectangle. Source bytes retained: 100%. | Actual text/high-colour and other selected mode paths, caller geometry, mode changes, transport failure and teardown. Three fixture traps explicitly reject untested text teardown/mono stretch services. |
| nt_ega.c | Partial. Original initialization owns LUT data; original PAINTFUNCS selects painters, which use original munge helpers and publish the resulting DIB. Source bytes retained: 100%. | Low-resolution and applicable alternative modes, real geometry/preconditions, concurrent updates, mode changes, publication failure and retirement. Distinguish original unsupported monochrome/user-font bodies from lost project functionality before selecting another provider. |
| nt_munge.c | Partial. Original interleaved source planes and initialized LUTs determine pixels; callers own source, destination and synchronization. Source bytes retained: 100%. | Establish real caller dimensions/address ranges and buffer lifetimes, plane-mask synchronization and mode-change/transport semantics. Passing bounded conversion cases is not complete guest rendering. |

The [per-file dispositions](planar-native-role-dispositions.csv) distinguish
100% retained original source text from whole restored behavior. A defensible
whole-code behavior percentage is currently unmeasurable: neither all mapped
functions nor a bounded pixel matrix measures every mode/failure/lifetime.
No artificial restoration ratio is inferred from successful case counts.

## Actual-object behavioral evidence

The [pixel contract](planar-pixel-contract-review.md) records **26,114 cases,
182,801 assertions**, using actual cached nt_cga/nt_ega/nt_munge objects.
All EGA masks and sample bytes, three scales, pitched source rows, rectangles,
source preservation, DIB guards, repeated initialization and null-output
guards are covered. The negative expected-pixel oracle is rejected at the
same real publication boundary (exit 1, one callback, two assertions failed).
The fixture owns memory and its invalidation sink; it creates no real window
and modifies no desktop, keyboard, product image or guest.

Source review also finds do/while conversion loops whose valid dimensions
must be supplied by callers. That is an explicit precondition investigation,
not a proved real-program zero/negative-size defect. Resolve callers and
actual reachable values before proposing a compatibility change.

## Delivery boundary

The [current gate](planar-native-delivery-verification.ps1) verifies exactly
these three row changes across all four shards, preserves every other row,
re-runs image binding, checks input hashes and retained native receipts, and
checks both ten-image packages remain unchanged. Documentation gates and
CURRENT's 16-KiB limit apply. Historical exact-count gates are run only in
their corresponding checkout.

No mirror source, product/guest binary or exception registration changes.
The candidate's unexplained SOL Options failures remain unresolved. This
checkpoint does not qualify it for publication or close S10/T473.
