# T479 S6 P1 pre-import preservation checkpoint

Owner explicitly requests a clean committed worktree before stacking adjacent
worktree commits; combined compilation, testing, publication and push follow
that import, before continuing S6. This is a preservation checkpoint, not S6
closure or a release-qualified runtime delivery. The standing goal remains paused.

## Included changes and review

- Candidate VM character/attribute fills use existing staged text and guest
  acknowledgement. They cross rows, clip at grid end, preserve the other cell
  component and cursor, and reject unrepresentable attributes. Application
  dispatch includes both operations in its existing transaction path.
- Unit tests cover cross-row content, clipping, zero count, invalid coordinates,
  unsupported attributes, cancellation rollback and retained successful state.
- The integration option substitutes fill operations for a known single-cell
  mutation; it retains original guest readback and normal-exit assertions.
- The baseline47-operation inventory and finite S6 audit are retained. Their
  seventeen unsupported-case count describes572d854a3, before the two candidate
  fill cases; it is not a count of today's missing cases or accepted support.
- Queue candidates8/9 and their two proposals are included at the owner's
  request. They are research candidates, do not admit implementation or broaden
  S6, and preserve original guest immutability and current COMSPEC semantics.

## Verification and unresolved failure

Cached worker build001 succeeds; mvdm-vm-text passes. Actual Win3.1 fill002
fails: startup and desktop/mode pass, normal exit does not. The installation
hash check confirms original bytes unchanged. Root cause has not been located;
this failure must be resolved in combined validation, not skipped or called a
pass. Its wrapper's final message says probe216 because it was copied from an
older runner; the retained invocation, output paths and matrix identify fill002.
[Evidence hashes](m0-t479-s6-pre-import.json) retain the failed result.

No product binaries are staged or published by this checkpoint. Both released
targets retain P4 package6c79f45b-88bd-4359-a9c8-15e3a753457e. Original guest and
mirror files are unchanged. All pending runtime changes remain candidates.

Next: identify the requested adjacent committed range, stack its history onto
this checkpoint, resolve code conflicts, then perform the requested combined
cached full build, failure correction, tests and coherent publication. Do not
start new S6 feature work before that integration delivery.
