# M0 T454 S3 — resource free, rollback and retirement witness

## Conclusion

S3 closes the bounded W05 free/rollback/retirement edge.  The selected
original `wres16.c`/`wres32.c`/`wkman.c` provider remains intact: it owns
`RES`, direction-specific callback results, raw task keys and resource release.
No guest source, guest binary, resource manager, PE fallback, alias allocator
or resource drawing path changed.

The real stress witness exposed two project-added return-time view projections
which were outside those original contracts.  A modeless `FindText` or
`ReplaceText` returns its native `HWND` directly in the original owner, while
the current adapter had also re-enumerated the whole desktop and borrowed its
client graph.  The same generic projection ran after terminal
`DestroyWindow`, after native USER had consumed that `HWND`.  Either projection
could convert a normal modeless lifetime into an adapter failure (`1400` during
creation, then `5023` during final destruction).

The repair is deliberately narrow:

- `wow_user_native_call_end_no_publication` retains the existing native-call
  scheduler/runtime restoration and native result/error handling, but omits
  the synthetic global return publication.
- Only the two original modeless common-dialog leaves and `DestroyWindow` use
  that exit.  Modal/file/font/colour/print leaves keep their existing return
  publication.  `DestroyWindow` still returns native USER's `BOOL`.
- The original `WCD32FindReplaceText` callback, original resource cleanup,
  `FreeRes16`/`DestroyRes16` and task-retirement algorithms remain selected
  unchanged.

This is an adapter-boundary removal, not a new lifecycle authority: real
window existence stays with native USER; original `RES`/guest resource
accounting stays with the selected original providers; W03 remains the sole
allocation/alias owner.

## Executed evidence

| Evidence | Result | What it proves |
| --- | --- | --- |
| `build/t454-s3/common-dialog-boundary-r3` | `COMMON_DIALOG_BOUNDARIES errors=0 leaves=7 scenarios=42 guest_proof=0` | All seven wrappers preserve one native invocation, nested/failure/SEH finalization; modeless leaves finalize without a synthetic publication. |
| `build/t454-s3/guest-resource-r16b` | pass | Immutable guest executes sixteen explicit modeless create/destroy rounds and one final live dialog, then retires once while the same WOW worker keeps WINMINE playable through flag/reset/open/close.  `CDL2` has 19 records, every destroyed dialog reports `DestroyWindow=1` and `IsWindow=0`, and the final trace has one `TaskRetirement ... stage=complete status=0`. |
| `build/t454-s3/component-build-r1` | `FULL_COMPONENT_BUILD_OK images=10 intermediates=1493` | Full x86/x64 component closure builds; a second dependency-driven build recompiles no intermediate or image. |
| `build/t454-s3/product-r2` | pass | Coherent ten-image candidate passes retained WINMINE/SOL/WRITE frontiers, Console-17 and Window-17 command groups with unchanged package identity. |

`build/t454-s3/task-lifecycle-r1` is retained as a non-pass, not accepted
evidence: its broader real-dialog fixture reached all its dialog markers and
then failed the unrelated final registered-owner cleanup assertion with error
203.  It was not the S3 resource-provider contract and no causal repair is
claimed.  S4 owns whether that broad foundation assertion needs a separate
source trace; it does not weaken the S3 witnesses above.

## Publication and media

The full-build candidate was staged as `build/t454-s3/package-r3`.  All ten
hashes match between that candidate, `assets/release/` and
`O:\winnt\system32`; the previous release is recoverably retained under
`build/t454-s3/release-backup-r1`.  The changed WOW32 hash is
`203D17B5F3F1C35C1243688D11A47A2B363A77D87209285F8C80889C2B051D01`.
Other host images were synchronized from the same full-build candidate.
`assets/winnt.zip` and all guest inputs remain unchanged; there is no guest
image diff or exception entry.

## S4 handoff

S4 may aggregate the selected lookup/load/lock/free foundation only.  It must
not absorb menu/dialog conversion, resource drawing, bulk module retirement
or a replacement resource manager.  The one retained broader real-dialog
fixture non-pass is explicitly a source-trace decision, not evidence that the
S3 selected free/rollback path failed.
