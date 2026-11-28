# S15 P69 diagnostic mapping and N023 dependency audit

## Scope

Start HEADe86baff61/runtime79. P68 completes N02239/39, S15 10/14 and
58/159 directories overall. This batch corrects a discovered diagnostic-line
placement error and performs the ten-file Full checkpoint. N023 migration is
not yet delivered; its first file requires the coupled ABI work below.
No original guest modification/rebuild.

## Diagnostic correction

Source: XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
windows/core/ntuser/client/rtlinit.c, blob6ebeb702a753755f12092737569b02ec29f5c908.
P66 placed four #line directives inside excluded DBG branches. In the actual
non-DBG build those directives never execute, so the next statements acquire
incorrect diagnostic line numbers. A physical-text sequence check missed this.
Move each directive after its original #endif and set it to the following XP
line. Preserve all four original DBG conditions and #error exclusions. No
function body, allocation, synchronization or callback behavior changes.

Use the real product compiler flags to preprocess the final translation unit.
The preprocessed result places allocation at322, return at334, zone membership
at356 and heap release at364, matching the pinned XP original. Evidence:
build/t485-s15/n023-ntcftxt/rtlinit-final.i and selected-line-audit.json.
Nonerror mirror metric remains6points/11lines, delta0/0. This also resolves the
P68 recorded follow-up; the previous textual-only claim is not reused as proof.

## N023 first-file source and ABI findings

The old inc/ntcftxt.h is not absent from XP. Its owner moved to
client/ntcftxt.h, blob e1fad45a3432fb5b1f3dfd59e74ed9163958738b,1763lines.
Current OpenNT-relative source remains1point/2005lines; no migration is counted.
The sole product include is wow32-dll/source/wow_class_client.c. The other
consumer is the original class-registration fixture. An old extraction script
still assumes OpenNT hashes and line shapes; it cannot prove XP fidelity.

XP RegisterClassExWOW replaces the old worker argument with flags and passes
both original and versioned class names to NtUserRegisterClassExWOW. It also
sets CSF_WIN40COMPAT from the module version and adds class-name capture cleanup.
Current wow_class_client_publish accepts an old PROC argument and only flags2;
changing the include without updating this provider would be an ABI regression.
The actual internal old _RegisterClassEx still owns registration/lifetime and
must not be bypassed with a new independent registration state machine.

XP GetClassInfo uses BEGINCALL_CLASSV from the XP ntsend.h. Therefore ntsend.h
is a required same-batch dependency, not an optional future cleanup. Its XP
blob is520ed2ce9d9b2f46905dc7360e2789fa0a3e87b6. The existing current-task
pClientInfo already provides dwTIFlags and dwCompatFlags2; class registration
must use that owner instead of adding another shadow task flag.

ClassNameToVersion is provided by XP client/classc.c:360 onward, blob
13ee43d5ccdcae28f5c8114b0de1a6c52f26bb60. It performs real activation-context
lookup and does not simply return its input for all Win16 callers. Its original
SXS declarations are already imported in sxstypes.h and nturtl.h. Preserve this
algorithm through finite native providers; do not substitute a success stub.

XP kernel/ntstubs.c:8890-8949 proves NtUserGetAtomName and clipboard-format name
lookup share UserGetAtomName. However clipboard lookup rejects integer atoms;
the original integer-atom behavior is separately established in the pinned
base/ntos/rtl/atom.c, blob c523186b0fc74daa9a8f3b058b1af5a17a38ecb6,
RtlQueryAtomInAtomTable:868 onward. Atom zero fails;
valid integer atoms format as #decimal, with bounded copying and termination.
String atoms use the USER atom table. A clipboard-name API alone therefore
cannot implement the complete provider. Preserve this finite distinction;
do not substitute a global atom table or create a second class registry.
The source evidence defines the contract, but no new provider is implemented
or runtime-validated in P69. Activation-context, string/atom, allocation failure
and cleanup routes remain the next migration batch's implementation work.

## Next coherent migration batch

1. Relocate the mirror to XP client/ntcftxt.h and update real product/test includes.
2. Migrate the required ntsend.h capture macros, preserving cleanup semantics.
3. Reuse the original class-name versioning provider with bounded actual native
   dependencies, or record a concrete unavailable boundary before a deviation.
4. Update the registered publication ABI as one chain; preserve the existing
   class owner, Win16 procedure identity, flags, lifetime and failure rollback.
5. Replace obsolete OpenNT-only source assertions with pinned XP evidence and
   run actual class registration/query/unregistration plus guest window tests.

This is an audit plan based on current providers, not a claim of implementation.

## Verification and census

Cached full build, runtime80 dual13-image26hash publication and final-package
Full24/24 passed. The compiler preprocessing check verifies the actual selected
line mappings. [Receipt](n023-ntcftxt-review-result.json) and
[census](n023-ntcftxt-review-census.json) retain the final package identities and
all gate results. This is the ten-file checkpoint following P61/runtime70.

Current census:1330files,789byte-identical,800nonerror-identical,530different,
7971points,66379lines. All six deltas zero;1307XP/23OpenNT. Directory progress
remains58/159, S15 10/14 and N0230/4. No original guest modification/rebuild.
N023 dependency findings above do not count as a completed migration.

The subsequent migration is delivered in [P71](n023-class-abi.md).
