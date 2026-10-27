# S10 current-source candidate verification

This is a candidate review, **not a release or S10 closure**. The candidate
contains the five runtime mirror updates committed by the concurrent setup
work in `35424a126`; `6ecbf166e` subsequently restored the independent path
repair tool. The shared-include checkpoint `10ce12242` changes no runtime
source. The accepted installed baseline remains `build/package-uuid/package-c`.

## Exact inputs and completed evidence

The [evidence index](candidate-runtime/evidence-index.json) binds the copied
reports, source hashes and tested ten-image manifest. Candidate package UUID
is `d1be0f65-adb4-4ee4-ae16-f301ad3f3c90`. No original guest was modified.

The actual MSVC x86 Ninja/ccache configuration rebuilt the affected worker
and providers, and its second worker/WOW32 build reported no work twice.
The native components reuse source-identical previously accepted artifacts.
All ten images were stamped and checked as one candidate package.

Completed checks on that exact candidate:

| Check | Actual outcome and scope |
| --- | --- |
| Full selected product suite | Pass, 331,253 ms: selected WOW frontiers, Console17, Window17, RPC/control, native routing/negative cases, handoff, relaunch and isolation gates. This is not every historical or T-wide capability test. |
| WINMINE gameplay | Pass using the existing isolated immutable-game probe. |
| SOL gameplay | Pass using the existing isolated immutable-game probe. |
| WRITE lifecycle | Pass: normal close, restart and second normal close. |
| Actual Video7 packed painter | Pass: full frame, pitched rows, clipped damage, source-end, invalid input and guards; controlled host framebuffer boundaries, not all guest mode-selection behavior. |
| Video7 BIOS probe | Pass: 32 legal settings, preserve/clear and two invalid requests. Unchanged independent DOS probe, no injected input. |
| SOL Options candidate | **Fail twice**: no completed modal dialog; trace records error 5023 during explicit creation. |
| SOL Options accepted baseline | Pass twice, interleaved in opposite order with the candidate. Same runner and desktop-capable launch context. |
| Error-location diagnostic | Pass once with a nonpublishable DLL made from three project-owned source copies and error-only observation. This is not proof that either production failure is fixed. |

The first BIOS invocation did not reach NASM because its wrapper accidentally
combined multiple resolved executable paths. That is a wrapper failure.
The second observer exited normally but exposed only 53 columns and clipped
the assertion marker. The third used the existing observer SHORT_HISTORY
private-Console geometry fixture and passed the complete unchanged assertion.
Both earlier attempts remain non-pass records, not product failures or waivers.

## Concrete unresolved publication boundary

In the first production candidate run, explicit class `8002` creation fails
with `bindingError=5023`. In the second, that dialog and ten controls are
created, but control ID `0134` fails with the same binding error. Both accepted
baseline runs complete modal creation, close and parent recovery. The ordinary
gameplay and WRITE passes therefore do not establish dialog correctness.

The existing `wow_window_creation_binding.c::claim_explicit` failure paths
cover enrollment, encoding/facts publication and original-client-state
creation. `wow_window_words_binding.c` and `wow_user_object_bindings.c`
contain the corresponding generation, ownership and publication checks.
The trace does not yet identify which internal invariant failed. Do not
delete a check, invent success, blame the desktop, or attribute causality to
the new real-mode CPL update merely from this comparison.

Static PE section comparison finds WOW32 `.text`, `.data` and `.reloc`
unchanged between accepted and candidate packages; NTVDM's executable
sections differ. This narrows the comparison, but does not exclude a
pre-existing timing/handle-reuse defect in unchanged provider code. The
error-only diagnostic's pass is sensitive to changed instrumentation/layout
and is deliberately retained as inconclusive.

Continue with a source-based distinction between native window enrollment,
the worker page-domain generation publication and the newly rebuilt worker
changes. All five mirror diffs remain partial; their source-policy local
markers, exception scope, minimal-diff comparison and affected real/protected/
V86 and Win3.x evidence still need final review. The successful painter/BIOS
checks do not satisfy those separate execution boundaries.

No candidate or diagnostic image was copied to `assets/release` or installed
`system32`. Publishing this candidate, pushing a production delivery, or
claiming the T complete is not justified by these results. The S10 source
reconciliation continues independently; its 1,890 pending and 236 partial
paths have not been promoted by this runtime verification.
