# S10 shard 2: COMMAND state, PIF and pipe redirection cohort

## Original/current differences and ownership

Read-only comparison against frozen OpenNT source yields:

- `cmddata.c` removes three process-global raw guest aliases (`pSCS_ToSync`,
  `pSCSInfo`, `pIsDosBinary`). The current session-thread guest-state owner
  records numerical locations and memory/session epochs instead. The original
  COMMAND scalars and decision logic remain distinct from that lease carrier;
  this is a changed mirror file, not a fully identical source.
- `cmdpif.c` retains original `cmdCheckForPIF` and changes its PIF-selected
  `*pIsDosBinary=0` write to `mvdm_command_guest_state_write_is_dos_binary(0)`;
  failure sets `pvi->ErrorCode=ERROR_INVALID_ADDRESS`. The adapter requires
  the correct active session, valid numeric location and guest-memory epoch,
  acquires a one-byte write lease and commits/releases it. This failure path
  is an added host behavior, not an original raw-alias result. The function
  still proceeds through original console-title/subsystem steps before cleanup;
  whether callers propagate that failure in every launch path needs direct
  caller evidence, not just the adapter return value.
- `cmdredir.c` inserts the DIV-120 local CreateThread boundary and replaces
  two casts for `cmdPipeOutThread` and `cmdPipeInThread`, retaining the original
  pipe bodies. The WINAPI thunk supplies a thread return and binds/unbinds the
  creator session around the original cdecl `void(LPVOID)` start routine.
  Original pipe EOF/events, temporary files, wait/cancel and close behavior
  remain COMMAND-owned; the adapter must not become another pipe owner.

The README registers the state cohort as DIV-111 and the pipe thread ABI
boundary as DIV-120. The retained product link map lists `cmdCheckForPIF`
from cmdpif.obj and both pipe threads and redirection entrypoints from
cmdredir.obj; extraction does not establish every scenario reached them.

## Evidence and bounded remaining work

T473 S4 documents selected COMMAND/XMS lifetimes and references original
command scalar/native-child fixtures. The scalar fixture exercises session
switch, epoch loss and lease failure using a memory fixture; it does not by
itself run PIF startup. The original PIF caller OEM fixture calls the actual
`cmdCheckForPIF` body and covers successful PIF, non-PIF and path/extension
cleanup branches, but its guest scalar writer is a test stub. T472 S3 audits
PIF Abort/Edit handling; it is source-level evidence for that boundary, not
an all-error runtime proof. T427 S2 establishes original temp fallback policy,
not all redirection thread retirement or pipe-I/O failure semantics.

A bounded follow-up should match a production-linked PIF caller to the actual
session lease: verify successful write, missing/stale session, failed lease
and commit, caller-visible `ErrorCode`, and cleanup without title/state
success being mistaken for successful launch. Separately exercise both original
pipe workers through the real thread adapter with input/output close,
creation failure, cancellation and session retirement, with actual observable
handle cleanup and callback/exit behavior. Existing nested-guest test failures
must retain their own environment and original-guest attribution; none is
assumed fixed or reproducible by this static review.

No production change or fresh test was made. These three modified source rows
remain partial; their behavior is not quantified by textual similarity.
