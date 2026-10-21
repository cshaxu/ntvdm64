# T448 S7 shared-library import compatibility audit

## Scope and inputs

Owner adds S7 after accepted S6: first audit unchanged-import incompatibilities
for discussion. Single-person implementer/reviewer. Local7d839ed96, unchanged
product d5d0dc243/protocol64. Old SoftPC pin3186afcd7b743e9797364c6a762eccd3b5576ca1;
new committed audit reference9ebb988776d0ef6f21b8aeb0e5ef93d5f2afc220.
New revision is not yet qualified for import. Dirty upstream WIP is excluded;
its frame_exchange lock_latest/unlock-to-observe_latest change is uncommitted.

Procedure: git show/diff on pinned upstream interfaces/implementations, local
consumer tracing and manifest hash comparison (all42 entries matched). Current
closure is types/base/kvm-base/kvm-window, not kvm-console. The earlier51-file
diff covered upstream directories, not the final selected closure. This is
static source evidence, not candidate compilation/runtime/performance proof.
No production/guest/mirror changes. SoftPC read-only. Temporary owner is S7,
build/t448-s7,100MiB, one hour/run, ten-minute no-progress investigation limit;
only hidden documentation checks are planned in this audit phase.

## Compatibility matrix

Local paths below are relative to src/ntcon-exe.

| ID | Evidence and unchanged-import consequence | Disposition proposed |
| --- | --- | --- |
| C1 text | Local lib/kvm-base/frame_interface.h supports160x96; upstream80x50. text_frame.c and window_frame.c use styles/secondary_cursor fields absent upstream. Underline and split cursor would be lost; font32 is now supported upstream. | Preserve capabilities via minimal registered extensions or upstream them first; caller renames alone are insufficient. Do not flatten all text into graphics to hide missing semantics. |
| C2 ownership | window_controller.c publishes a copied frame; frontend_session.c reuses one allocation. New publish_frame borrows a four-slot exchange; payload pointers belong to slots. | Adapt NTCON producer ownership; no shallow copy/early free. Stop production, join Window, then release exchange. Test CAF recreation, stale publication and failure. No pointer crosses IPC. |
| C3 graphics | window_frame.c produces1/8-bit indexed pixels plus palette; new Window accepts32-bit0x00RRGGBB. | Convert inside NTCON, retain worker wire ABI. Four maximum1280x768 RGB slots alone contain15MiB pixel payload; measure memory/conversion cost before claiming improvement. |
| C4 cursor | cursor_visible becomes DISABLED/HOST/SOURCE. Local text_frame.c declares Window-owned blinking. | Explicit HOST/DISABLED mapping, preserve secondary shape through C1; no second phase owner. |
| C5 input | Keys and decoder move from KVM to Base; window_keyboard.c uses old KVM names. New capture coordinates raw registration and checks focus. | Select Base input/type closure and update consumers; test CAF, repeats/Unicode, reset/retirement, focus rejection and rollback. Failures retain NTCON-to-NTSRV reporting, not independent exit authority. |
| C6 join | Local worker_interface.h has LIB_UINT32_MAX; new upstream still has DESTROY_TIMEOUT_MS5000u for normal/startup-failure join. | Preserve approved no-extra-timeout exception. This is distinct from original Console-close grace; raw import reverses T448 policy. |
| C7 dead APIs | S6 removed unused task/event/wait/default-notifier APIs; upstream retains default wait selection. Exchange needs mutexes, not all task APIs. | Re-audit new dependency closure; retain pruning where unused rather than restoring dead code for byte equality. |
| C8 build/provenance | New Base input/text and exchange units are outside old selection. New-T310OriginalSoftpcNinja.ps1 consumes manifest rows; profile text still says x86 though NTCON ships x64. | Update selection/hashes/notices/architecture, relevant tests only. No Console/broker/audio/storage/Linux product integration or SoftPC runtime dependency. |

## Owner decisions and recommendation

Recommend committed upstream core plus NTCON consumer adaptation, retaining
explicit text-capability and no-extra-timeout exceptions. Exact unmodified
upstream cannot meet current requirements. Alternative: extend upstream first
and wait for its qualified delivery. Do not silently lose features or restore
timeouts. Adopt exchange/RGB locally without changing worker transport or NTSRV
authority; conversion/memory tradeoff is explicit, performance still unproved.
Pin a committed qualified revision; current HEAD is only an audit reference
until upstream qualification evidence is checked. Never mix dirty WIP.

Ordinary key/type renaming, manifest correction and dependency selection need
engineering verification, not new semantic concessions. Existing ownership and
timeout prohibitions remain binding without repeated approval.

## Next implementation boundary

After decisions, revise packet for one grouped library/NTCON frame-input-
controller/build-manifest/test migration. No NTSRV, worker wire, guest, emulator
or NTW16S changes planned; explain any proven expansion first. Exact line budget
requires selected closure and exception design, not the directory diff count.

Acceptance: library tests;160x96/font32/underline/split cursor; indexed and
palette-only conversion; latest-wins/failure/retirement/reopen; input/capture;
frontend/channel/text and eight CAF pre-Enter echo cases; full13/incremental and
coherent two-destination publication after runtime changes. Audit delivery is
documentation-only, no binaries. S7 stays open pending decisions.

## Removal research requested by owner

Read-only source tracing establishes live producers, not just unused fields:

- NTVWM text_frame.c::ntvwm_text_frame_pack reads actual viewport dimensions,
  accepts up to160x96 and converts COMMON_LVB_UNDERSCORE into optional third
  bytes. frontend_window_library_test.c covers120x30,160x54,160x96 and exact
  pixels; ntvwm_text_frame_test.c tests the native attribute mapping. These
  are targeted fixtures, not evidence of a particular everyday app requiring
  every maximum dimension.
- Original mvdm/softpc.new/base/video/vga_prts.c::do_new_cursor produces a
  second segment when cursor end is below start (with distinct zero-end handling).
  mvdm_softpc_text_video_copy reads both segments; win32/console_text.c publishes
  them; NTCON window_frame.c/text_frame.c forwards them to its extended renderer.
  Therefore removal changes a source-defined display behavior, not dead-code
  cleanup. No guest or original emulator change is necessary or proposed.
- frontend_session.c processes Window decoding even outside active Window mode;
  decode errors propagate through its pump. Reducing a library constant alone
  can fail presentation rather than merely disabling CAF. Window capacity must
  be separated from protocol validity if only the Window feature is restricted.

Least-scope removal option: leave the established wire and original producers
intact, discard underline and secondary-segment presentation only at the NTCON
Window conversion, and remove those library fields/render/geometry branches.
Native Console underline currently maps back to COMMON_LVB_UNDERSCORE in
frontend_session.c and can remain. This deliberately makes Window less faithful
than existing behavior; collapsing two segments to one bounding block is not
equivalent. Prefer the primary segment if such a visual concession is approved.

Large text has no equivalent lossless80x50 representation. Options are explicit
Window-mode refusal with continued Console presentation, approved crop/scroll
interaction, or a product-wide80x50 restriction. Refusal must handle growth while
already in Window mode, avoid leaving a stale picture, preserve data and input
coordinates, and keep control with NTSRV. It is a feature decision and requires
implementation design/tests; simply returning ERROR_NOT_SUPPORTED from decoding
is unsafe as a purported nonfatal refusal. Do not resize hidden guest/native
Consoles or change guest video modes just to accommodate a library limit.

Full product removal is larger: remove native attribute production, style-byte
consumers, geometry acceptance and protocol fields together, revise the wire
version and all peers/tests. It still cannot delete original VGA behavior.
Not recommended merely to avoid three local library extensions. No new removal
is implemented or runtime-tested; owner request here is research only. Existing
positive tests must become explicit approved-degradation tests, never disappear
silently. No evidence yet establishes the exact160x96 maximum as mandatory.
