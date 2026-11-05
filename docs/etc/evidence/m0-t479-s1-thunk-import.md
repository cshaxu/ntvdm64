# T479 S1/generic-thunk import

Owner authorizes all committed adjacent work and permits topic-qualified
repeated S labels. Import source: task/mirror-source-audit, e1867aaa2 and
cd91c8a57, common ancestor cd8c8583e. Use a merge to retain both original
commit identities. The adjacent worktree is clean and remains untouched.

The source S1 inventory/findings and S2 generic-call recovery are imported
in full. S1 inventory reconciliation is not complete. S2 selects the original
x86 call helper and preserves EBX, with original callconv.inc provenance and
MVDM-HOST-DIV-674/675 registration. Tests cover host and guest call conventions.
No guest source changes are present.

Only documentation conflicts occurred: combine indexes; retain local T479
active authority and T477 delivered history; remove the independently allocated
audit package from Queue without requeueing the already admitted guest bridge.
Retain all imported evidence and remaining finding-stage plan. The old
ca910d6a1 identity in evidence describes the pre-rebase local commit; the
imported post-rebase source commit is e1867aaa2.

No foreign release asset changes are in this commit range. The installed
T477 package remains until a coherent combined-source package passes gates.
Integrated build/tests/publication are pending, not inferred from source
worktree success. Temporary evidence: build/t479-s1-import, 90-minute phase,
4 GiB, 15-minute no-progress review budget. Guest S1 contract work is not
closed or advanced by this side-work integration.

## Integrated verification attempts

The first cached build used direct Ninja against the existing WOW graph.
The helper sentinel test passed, but actual-thunk linking failed with LNK1181
because callpr32.obj was absent: the new generator had not been run. This is
a failed runner attempt, not product acceptance. The corrected invocation is
cmake --build build/.ninja/msvc-x86-release --target wow32, which regenerates
the selected graph before compiling. It exited zero; the actual product
wkgthunk/callpr32 object test then passed all 12 checks with errors=0.
The separate helper test passes ESP/EBX, cdecl/stdcall, nesting, zero arguments
and propagated exception checks (errors=0, exception=1). Existing x86/x64
product builds passed. Final integrated guest/Full verification is in progress.

Imported callconv.inc SHA-256 matches the recorded original exactly:
044E0712A074C7FD61E1A4CA04FAC1FF860F028D289C92E320B01CC9C7690BB5.
No globally enabled i386 flag or independently rewritten call algorithm is
introduced. The local document governance/self-test passed before final
result recording. The initial failed runner logs remain preserved.

The first integrated Full attempt passed guest-media, exports, WOW startup,
Console-17, Window-17, RPC and native-GUI gates, then correctly rejected the
stale APP_VERSION 0.0.477 against admitted T479. This was an integration
omission: update only application identity to 0.0.479 (protocol stays 67),
rebuild the coherent package and repeat validation. No failing package was
published. Both real Win16 normal/mismatch tests passed on that first package;
new-package results must be recorded separately rather than inferred.

The corrected 0.0.479 x86/x64 builds exit zero. Both focused helper/thunk
matrices pass again. On the new p3 package, normal and mismatch guest cases
both report three GEN1 phases, zero errors and three resource releases.
The 42-case service suite passes on each architecture; six frontend fixtures,
frontend-base isolation and VM registry/lifecycle assertions pass. Final Full
runner PID 19044 started 2026-11-05T23:31:42.9484988Z; completion is pending.

## Final integrated delivery

Final runner PID 19044 exited zero. Full passed 16/16 selected gates in
382099 ms on the identical ten-image package. Version-negative checks
pass with APP_VERSION 0.0.479 and protocol 67. Publication exited zero;
assets/release and O:/winnt/system32 both match the tested manifest, package
cdbe0b52-73bf-4a30-9ea2-abf9f3a9abf3. The committed release-manifest.json
retains the ten hashes. Prior failed attempts are not included as passes.
DOS4GW/NESticle retains its previously approved exclusion; WOW startup does
not prove gameplay. The source worktree's real exception-sibling acceptance
is retained as upstream evidence, not claimed as a newly repeated case here;
this integration repeats helper/thunk SEH and normal/mismatch guest calls.

Production changes imported unchanged: mirror source +345/-6, including the
303-line byte-exact macro include; exception registers are separate +6 lines.
Tests +138/-1 and build generator +5/-1. Local integration additionally changes
APP_VERSION (+1/-1), documentation and the coherent release package. No guest
source or original guest binary was modified. The merge retains e1867aaa2
and cd91c8a57 as ancestors; provenance notes referencing ca910d6a1 are historic.
S1/generic-thunk-import is delivered; S1/guest-bridge remains active. Source
S1 inventory limitations and outstanding T478 findings remain explicit.
