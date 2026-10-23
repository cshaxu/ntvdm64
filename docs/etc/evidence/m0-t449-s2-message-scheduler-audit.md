# T449 S2 — M1--M3 message and scheduler source audit

This is a source-first working audit of the current DDE candidate, not S2
closure or a runtime pass. It maps the real selected path before another
message-family repair is considered. Guest images are unchanged.

## Selected production path

`mvdm/wow32/precomp.h` defines `WOW_USER_PRIVATE_REDIRECT` before including
`wow_user_private_access.h`. Thus the original `wumsg.c` calls to
`GetMessage`, `PeekMessage`, and `WaitMessage` bind to the project adapter;
they are not unwrapped modern USER calls. The path is:

```
original WU32GetMessage / WU32PeekMessage / WU32WaitMessage
  -> wow_user_get/peek/wait_message
  -> wow_user_task_lifecycle_message / wait_message
  -> original taskman.c xxxSleepTask / xxxUserYield / xxxDirectedYield
  -> host wait operation: current-thread USER queue plus original task events
```

The adapter is not a second posted-message queue. It initializes the native
queue with a non-removing peek, dispatches only pending synchronous sends with
`PM_QS_SENDMESSAGE`, and leaves ordinary posted retrieval, filtering and
`WM_QUIT` to the original thunk's selected native call. This is required
because original `xxxReceiveMessages` depends on unavailable NT4 SMS/PWND
state. It is the already-approved modern USER boundary, not an alternative
WOW scheduler.

`WU32InSendMessage` and `WU32ReplyMessage` deliberately retain their original
direct USER calls. `ReplyMessage` is not redirected or wrapped in a fabricated
receiver identity: the host owns whether a synchronous sender exists and the
return value. Its original comment already treats all flat pointers as invalid
after the call. No project code sets the private `psmsCurrent` or `psmsSent`
fields; the original taskman branch that uses them is consequently not claimed
to run on modern USER. This is the approved loss of NT4 private-SMS ordering,
not a missing connection or a reason to manufacture an SMS object.

## Ownership and failure review

| Concern | Current owner and conclusion |
| --- | --- |
| Native sent-message admission | `wow_user_message_bridge_receive[_wait]`: public USER `PeekMessage` dispatches sends without removing guest posted messages. The bridge verifies the bound current thread and never supplies a receiver target. |
| Task selection and execution | Imported original `taskman.c`; its task lists, event counts, priority and directed-yield policy remain selected. `wow_task_execution_scope` is a LIFO native-stack eligibility guard, not a message identity or queue. |
| Native call return | `wow_user_native_call_end` restores the execution owner, runs original directed yield when needed, then publishes the guest view. A failed required restoration terminates rather than continuing on partial state. |
| Idle bit | Original `BlockWOWIdle` remains one VDM-state bit. The DDE/message thunks bracket ordinary Get/Peek/Wait calls as original code did; this audit found no added polling or timeout path. |
| Private NT4 state | The adapter's explicitly bounded structs model only fields consumed by imported taskman. It does not cast a native USER object to `SMS`, `PTHREADINFO` or `PWND`. |

## Remaining finite evidence boundary

No source gap requiring a new M1--M3 implementation was found in this pass.
This does **not** prove all runtime behavior. The current-package witness must
still cover same-worker and cross-worker send/post, implicit and explicit
reply, nested callback return, pending wait, normal task retirement and the
selected failure modes. It also cannot restore NT4's private-SMS early-reply
ordering without an unapproved semantic change. Those facts are test/contract
boundaries, not an invitation to add a second scheduler or queue.

`message-cross-r1` supplied a historical M1/M2 normal witness for Off, Win16
and All styles: all three same-worker controls and all three two-worker cases
completed with exit zero. The package manifest pins all 13 host images; WOW32
was
`90C03F0D79041E36C1EC4F46AE5816CB2E42119D445BEC133225EE810DB19E33`, and
the guest is the unmodified canonical archive. The runner's `result.json`
records six passed cases. It is useful provenance for the named
scalar/nested/reply/post/filter and selected structure/ordinary-exit paths, but
it is not current-candidate proof after the later DDE ownership change. The
current candidate must rerun this group before S2 delivery; neither run proves
pending receiver death, every implicit receive branch, all converter families,
or private-SMS ordering.

`message-pending-r1` then passed six historical-package local/pending cases in
Off, Win16 and All, plus x86/x64 native destruction controls. Its required
negative endpoint fails with `reverse callback before blocked send returned`,
proving the harness rejects the forbidden no-block ordering rather than
mistaking it for a successful receive. This is the selected public-USER
pending-send/receiver-destruction contract. It remains distinct from NT4's
unavailable private SMS timing and does not prove every receiver-death or
reentry interleaving.

## Construction-peer provenance check

The first current-package construction attempt (`message-construction-r1`)
failed its final Win16-to-native 512-byte `WM_WINHELP` assertion.  The worker
and the forward native-to-Win16 call had completed, but r1 had explicitly
copied a 87,040-byte `MSGHOST.DLL` last written on 2026-10-18 rather than
building the peer from the current probe source.  Its result cannot establish
a WOW32 regression: P19's same 512-byte source assertion passed, while r1
combined different fixture revisions.

`message-construction-r2` rebuilt the peer from its then-current
`wow_message_peer.c` and passed against the staged package and WOW32
`935B25BD6955F3AB8DA7575320E258E9B416B29EEAAF210B121967B59B8CADD3`.
The final sender record has `errors=0`, all three MDI create/destroy rows,
the title/parameter/text checks, one forward WinHelp call/free, and the
512-byte reverse payload return.  The runner now rejects `-Construction`
with `-NativeHost`; construction must build its peer from current source.
The preflight parser and a stale-host rejection control passed without
starting an isolated runtime.  This removes fixture-version drift from this
specific construction gate. It is historical provenance, not proof for the
later DDE candidate and not a claim that every M4 conversion has been proved.

## Current DDE-candidate message matrix — 2026-10-23

After the bounded `WU32PostMessage` DDE ownership repair, three independent
actual-runtime groups reran against the coherent candidate package
`build/t449-s2/media-dde-r30`. Its changed `WOW32.DLL` is
`E1424CAC174C3388D293142A1A7F159634088BA07E65797408997A3B83C5BC39`;
the candidate's complete `SYSTEM32` CSV manifest was rehashed row-by-row
before any worker was started. The guest archive was the canonical unchanged
`assets/winnt.zip` asserted by the runner.

| Group | Result | Scope actually witnessed |
| --- | --- | --- |
| `build/t449-s2-message-cross-r8` | x86/x64 fixture builds; local/cross × Off/Win16/All: 6/6 exit zero | Same and distinct WOW workers, direct native host boundary, nested/reply/post/filter paths and normal retirement. |
| `build/t449-s2-message-pending-r2` | x86/x64 fixture builds; native receiver-death controls; local/pending × Off/Win16/All: 6/6 exit zero; required non-blocking negative exit one | Pending send and selected receiver destruction retain the required blocked-send ordering rather than accepting the forbidden early callback. |
| `build/t449-s2-message-external-r1` | x86/x64 fixture builds; local/x86-peer/x64-peer × Off/Win16/All: 9/9 exit zero | Both external native bitnesses and both directions through the selected public USER boundary. |

The runner initially assumed a JSON-only 13-image candidate manifest, while
the produced package correctly exposes a complete System32 CSV manifest. Its
test-only reader now accepts either formal representation, verifies every CSV
row is directly within candidate `SYSTEM32` and hashes to its recorded value.
No product or guest behavior changed for that correction. The retired inline
PowerShell release-row assertion was redundant once every full-package CSV
row had been verified.

This establishes the current-candidate witness for the named matrix. It does
not prove every conversion family, all allocation failures, arbitrary
interleavings, M4 construction data conversion, M5 targeted deletion, or the
approved unavailable NT4 private-SMS ordering.

The remaining M4 construction witness was then rerun against the same current
candidate from freshly assembled `MSGSEND.EXE`, `MSGRECV.EXE` and current
`wow_message_peer.c` in
`build/t449-s2-message-construction-r4`. It exited zero. Its terminal sender
record has zero errors with 3/3 MDI create/destroy rows, title and parameter
mutation, three text checks, one WinHelp call and one corresponding free;
the receiver records zero errors and four structured conversion checks. The
previous r3 attempt used Windows PowerShell, whose .NET implementation lacks
`ProcessStartInfo.ArgumentList`; it failed before launching a runtime and is
not product evidence. r4 uses the PowerShell 7 runner host already used by
the completed matrices.

This narrows the preceding statement: selected M4 construction data conversion
now has a current-candidate witness. It still does not prove all M4 conversion
families or failure branches, M5 targeted deletion, arbitrary interleavings or
the approved unavailable NT4 private-SMS ordering.

## P delivery package and final rerun — 2026-10-23

The fresh package root `build/t449-s2/media-dde-r31` was rebuilt from the
complete cached x86/x64 component set, not assembled from a partial test
image. `Test-FullComponentBuild.ps1` recorded all thirteen selected images,
correct x86/x64 PE identities, and `unchangedIntermediates=true` after its
second incremental pass.  The resulting WOW32 SHA-256 is
`E1424CAC174C3388D293142A1A7F159634088BA07E65797408997A3B83C5BC39`.

The same freshly staged root then passed the final package-scoped matrices:

| Evidence root | Result |
| --- | --- |
| `build/t449-s2-message-cross-r9` | 6/6 local/cross-worker cases, Off/Win16/All, exit zero. |
| `build/t449-s2-message-pending-r3` | 6/6 local/pending cases plus x86/x64 receiver-death controls; the no-block negative exits one as required. |
| `build/t449-s2-message-external-r2` | 9/9 local/x86-peer/x64-peer cases, Off/Win16/All, exit zero. |
| `build/t449-s2-message-construction-r6` | exit zero; 3/3 MDI create/destroy, title/parameter mutations, three text checks and one WinHelp allocation/free. |

`construction-r5` is not a runtime result: its relative compiler-driver path
was rejected by PowerShell before it launched a guest. `r6` repeats the exact
case with the required absolute driver path.

All thirteen staged files were backed up before publication, then copied as
one coherent set to `assets/release` and `O:\winnt\system32`. Every deployed
file was rehashed against the committed `assets/release/release-manifest.json`.
The immutable guest archive and guest/system configuration remain unchanged.
This P establishes the selected S2 M1--M5 execution/ordinary-retirement
boundary; it does not claim S3 per-task termination, all conversion families,
private-SMS timing, or whole-provider completion.
