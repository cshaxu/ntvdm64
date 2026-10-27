# Kernel 3.1 source-role checkpoint

This S10 batch follows the [COMMAND/WRITE/DOS role checkpoint](guest-source-role-review.md)
at `3fd833983`. Its [100-path table](kernel31-source-role-dispositions.csv)
classifies historical guest build-input roles only. It does not establish
that 100 guest functions work or that these sources generated the selected
immutable kernel image.

| Shard | Accepted file roles | Exact evidence |
| --- | --- | --- |
| 0 | 26 | [Object/link response, include and stub chain](shard-0/kernel31-guest-owner-review.md) |
| 1 | 24 | [KRNL386, separate 286P and SWAP tool](shard-1/kernel31-guest-owner-review.md) |
| 2 | 27 | [Explicit source/include chains and separate 286P inputs](shard-2/kernel31-guest-owner-review.md) |
| 3 | 23 | [Source/definition/RC chains and separate 286P inputs](shard-3/kernel31-guest-owner-review.md) |

The original makefile selects MASM/CL16/link16, retains separate OBJ2/286P
and OBJ3/386P link branches and supplies the resource/stub stages. Both actual
link response files were read; object/source basename similarity alone was
not accepted. NT's default remains 386. The historical 286 inputs are file
roles, not admission of another runtime profile. The separately built
`swappro/swap.c` belongs to SWAP.EXE, not KRNL386.

Literal include chains and RC references identify header/resource inputs.
They do not prove every conditional branch was active. In particular strings
can be copied from localized sources; LANG/COUNTRY/DBCS and DEBUG/DEST
conditions remain explicit. Unproved helpers, generated/resource binaries,
executables and image provenance stay pending. No original guest compiler,
linker, patcher or runtime image was invoked or changed.

All 100 current and referenced original files were independently rehashed
and match the frozen identities. No mirror-different file is excluded. The
[observation receipt](kernel31-source-role-observations.json) binds the exact
per-shard candidate tables and reuses the unchanged host input/dependency
snapshots from the preceding checkpoint; no kernel31 path occurs in them.
These negative observations supplement the original guest chains, not replace
them. Current configuration or future source selection changes require a
fresh review of this role boundary.

S14 retains kernel task/module, execution, thunk and lifecycle interactions
with the original host providers; S12 retains underlying DOS services;
S15/S16 retain dependent USER/GDI/resource contracts. Immutable-image
identity, source derivation and actual guest behavior remain distinct. No
capability or test is removed by excluding these source files as host inputs.

The current ledger contains 52 full, 226 partial, 1,753 excluded file roles
and 2,653 pending paths. The frozen 4,684-path inventory and source estimates
are unchanged. S10 remains open.

Verification uses the existing checkpoint checker with the new base and
disposition table, rather than a second implementation of the ledger rules:

```powershell
python -X utf8 artifacts/mvdm-audit/guest-source-role-checkpoint.py artifacts/mvdm-audit/kernel31-source-role-checkpoint-result.json --base 3fd833983 --prefix kernel31-source-role- --expected-count 100
```

This is source/ledger verification, not runtime acceptance or a release.

The first checkpoint run rejected a changed concurrent `nt_graph.c` hash.
The [source observation](live-candidate-source-observation.md#later-graphpalette-observation)
records the newer bytes and source-retention estimate; the existing partial
classification is unchanged. An updated observation does not constitute
runtime review or acceptance of that other session's candidate.

Independent read-only review rehashed all 100 original/current file pairs
and found no mirror-different exclusion. It checked the main-session source
chains, including `aliases.asm` as an actual include of `2protect.asm:3533`,
`kernstub.asm` as a separate stub prerequisite outside the main response,
the generated/copied strings input, and the separate SWAP tool. No withdrawal
of a role classification was required. This review does not establish every
configuration or runtime behavior.

The [checkpoint result](kernel31-source-role-checkpoint-result.json) passed
after the explicit drift refresh. A negative invocation with expected count
99 failed specifically at the disposition-count assertion, as required;
neither invocation is a product behavior test. The shared verifier takes a
base/table/count so this batch does not duplicate its implementation.

Documentation governance and `git diff --check` passed. The
[background run record](kernel31-source-role-runs.json) preserves the initial
drift rejection, subsequent observation, successful rerun and count-negative
result separately.
