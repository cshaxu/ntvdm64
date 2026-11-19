# T483 S13 implementation migration closure

The owner revised S13 from direct-edge isolation to actual inward migration.
P16 records the approved S13-S19 plan and withdraws the unpublished mouse
forwarding experiment. P17 completes the first bounded physical-mapping chain;
it does not claim the whole C layer or other service tables are clean.

## Result and ownership

Eight mapping operations now execute in compat/physical_mapping.c. The old
F provider, its private header and its installer, plus the B forwarding table
and accessor, are removed. Original C callers and all mirror source bytes
are unchanged. B owns the mapping records, alias operations and cleanup.

The existing current-instance TLS and cleanup record mechanism move to B;
there is no second current-instance registry. F embeds that context and retains
thread counts/hook sequencing, product activation/cancellation/completion,
termination escape and disposal eligibility. F opens/closes resource
registration at its existing transitions. The session accessor retains its
cdecl export and recovers the containing F object; the private structure ABI
version advances from10 to11. No guest or task protocol is changed.

The invalid-owner mapping test now corrupts B-owned cleanup metadata, rather
than F session magic. Mapping cannot depend on that private outer structure;
F retains its own session validation. Tests also cover alias-only ownership,
owner-address reuse, rejected registration after completion, bind rollback,
failed rollback/unbind retention, cleanup order and real cross-DLL binding.

## Evidence and limitations

[P17 evidence](../../../artifacts/mvdm-audit/t483-c-layer-dependencies/s13-p17-mapping-implementation.json)
records source identities, failed attempts, final receipts and22 published
hashes. Package92f469bf-1555-4ff0-b16d-ef2d9292934a is published to both
assets/release and O:/WINNT/system32. Cached x86/x64/reference builds,8 A/B
graphs and targeted tests passed. Only NTVDM differs after masking package
GUID and PE/debug timestamps. NESticle menu pixels and Escape hide/restore,
WOW page-domain CCPU, original memory, high-linear-page, session/thread,
alias and isolated DOS DIR checks passed. This is not Full or new manual
Win3.x/Win95 desktop acceptance.

The first two build attempts failed during graph generation: a stale moved
filename and a flag-definition ordering error. Both are corrected, retained
as failures, and superseded by the final successful cached build. The
withdrawn mouse candidate remains separate historical evidence.

Mirror cohort22 points/92 lines is unchanged; this batch adds/removes zero
mirror points/lines. Cumulative reduction remains3 points/12 lines. The
618-ID direct ledger remains144 eliminated/424 open/44 supporting/
5 eliminated-transitive/1 excluded. Nine existing eliminated rows now carry
actual implementation ownership evidence; they are not counted twice.

Under the owner's sequential execution approval, the bounded S13 closes and
S14 starts the remaining guest-memory/lease, XMS and WOW page-domain work.
The approved plan retains all other outward dependencies and service tables.
