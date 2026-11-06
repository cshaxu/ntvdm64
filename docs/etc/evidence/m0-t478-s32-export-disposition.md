# T478 S32: original export profile and consumer reconciliation

Parent: f4f5c0802. Finding 08's two missing exports were already connected.
S32 restores only the canonical export template's original line-ending bytes
and extends existing verification; no allocator or consumer implementation changes.
Coherent build, final live verification, Full and worktree publication completed.

## Source and profile

The original ntvdm.src selected i386/CPU40 metadata profile contains 140 unique
exports. Fresh MSVC preprocessing matches the cached original profile; all
140 names, aliases and DATA properties are retained in the generated provider
DEF. The i386 define here preprocesses declarations only; it does not enable
CPU30/kernel-monitor source or an alternate product executor.

The current contract and PE have 274 names. All 271 entries from recovery
commit a8a0aef7e retain their ordinals; three later task-debug exports occupy
272–274. Query/reallocate remain 270/271. Sim32FlushVDMPointer remains in the
project extension at its retained ordinal. Nine alias/DATA targets match the
actual linked image RVAs and function/DATA map kinds. CMake tracks the original
template and ordinal CSV, so changes propagate through generation.

The historical claim that the template was byte-identical had become stale:
current bytes used LF (BE81394020327C030107CBA92CB10FED5D7DE6DA5ED37DC46303D96ABEAD15D7),
while both pinned originals use CRLF (D47400179C1388B58FB8EAEC8331C6EEA16EE548BBA10E7AFBE588E9549BD0BC).
Their content differed only in the 236 line endings. S32 restores the exact
original bytes, with no changed declaration, token or code body. The separate
supplemental i386/ntvdm.def already retains its exact registered original hash
98C00C3C4737108674061779A6C1E06F113C597D76536A49E0723AA617D58A7A.
This removes one byte-level template divergence; it is not a global mirror
coverage or full runtime-functionality claim. Licenses/source roles unchanged.

## Supported consumers and retained boundaries

The current map binds query/reallocate to selected original nt_mem.obj.
The repository's production direct references are in DPMI memory code:
CPU40 reallocation intentionally returns through SAReallocate first; memory
information preserves the VDM query call but reports the shared XMS SAQueryFree
values. DIV-224 owns that established allocation family. Replacing it merely
to force use of a PE export would change ownership and is not justified.
WOW's allocation/free thunks use the original VDM allocator as recorded in S17.
No other repository production query/reallocate call site was found; exported
VDD consumers remain an external ABI, not a finite catalog of third parties.

The existing actual-worker VDD witness resolves both exports by name and
ordinal in GetModuleHandle(NULL), allocates, queries, rejects an oversized
reallocation without altering the original address/free-state observation,
then grows and frees successfully. S32 additionally records the exercised
DOS PDB and queries at its matching original termination callback. The runner
now requires that marker and rejects any FAIL line. It uses a private desktop
and a temporary package-local VDD overlay; immutable MEM runs unchanged.
This proves a bounded create/terminate lifetime, not automatic allocator
reclamation on arbitrary task death or every VDD/device/concurrent consumer.

The rebuilt original-external-memory fixture also passes grow/shrink/content,
name/ordinal calls, EMS/SAS sharing and real bidirectional DIB/GDI cases. It
uses existing controlled seams and /FORCE links; real worker evidence is
separate. S17's original commit/decommit rollback and malformed-address limits
remain recorded, not newly repaired or covered by this exhaustion case.

## Run accounting

Owned background verification first stopped before compiling because cmd was
given a forward-slash script path; corrected PID 35852 completed the native,
real-worker and metadata checks. Original profile PID 13532 succeeded.
An initial exact-byte assertion exposed the genuine inherited newline
difference; it was not waived as byte identity. The template was then restored.
A read-only Git lookup of the external checkout was denied by the restricted
shell; pinned file content and hashes were readable and sufficient for the
actual comparison. No external source was changed.

The fixture rebuild reported a Ninja log-version mismatch and rebuilt its
selected dependencies. Subsequent coherent product build PID 13896 uses the
established architecture runners. Test-only code never enters the release;
restoring this tracked generator input triggers the normal product build,
Full and worktree-publication procedure. Detailed logs remain in build/t478-s32.
No O:/winnt write or push is authorized. Earlier unrelated startup diagnostics
and DOS4GW/NESticle exclusions remain as recorded, not newly passing claims.

## Final delivery

Final package 3f06f0c6-d34a-4a6d-a862-4c89ed5373b2 passed the fresh profile and
nine alias/DATA checks, the real VDD/MEM create/termination witness and all
16 Full gates in 380,184 ms. The final supervisor was PID 17164.
Version 0.0.478 / protocol 67 remain unchanged.

The private live stage has a fresh identity. The helper copies the selected
package, checks every copied PE against the source manifest Sha256, then
restamps only the package identity and records the resulting hashes.
InputSha256 is inherited build-input lineage: the receipt initially compared
it incorrectly with the stamped source-package Sha256 and rejected that
comparison. Reviewing the actual helper resolved this evidence-accounting
error; it is not a product/test failure or a claim of identical resealed PEs.
The final runner/helper identities, source and live manifests are retained.

All ten files and the manifest were published to worktree assets/release with
exact tested hashes. Previous release is retained at build/t478-s32/release-before.
No O:/winnt write or push occurred. The
[durable receipt](../../../artifacts/mvdm-audit/shard-1/export-s32-results.json)
contains declaration/ordinal comparison, source/binary inputs, native/live
outputs, identity accounting, Full timings and publication evidence.

S32 closes finding 08: existing export recovery is effective for the selected
profile and tested supported host consumers. The source-format remainder is
removed byte-for-byte without autonomous runtime code. Original allocation
limits and arbitrary consumer/concurrency coverage remain explicit. Broader
T478 inventory reconciliation and subsequent findings remain open.
