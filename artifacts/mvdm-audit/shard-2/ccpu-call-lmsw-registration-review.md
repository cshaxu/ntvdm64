# S10 shard 2: CCPU CALLF/LMSW source and registration review

## Frozen and current source identity

The selected `call.c` compares OpenNT SHA256
`CE5F56EA27239D0C85243CEE23E01DE34B1F82D3BA92ECED687E2B3BD90A2A94`
to current `EA0E61F2EB649A8C4109611091DB3CF5D98E0600A425B31784591E9316AE2B0C`;
`lmsw.c` compares `8BC5BAD26249B91C1CC9B3BE756575C081B2DA322BB8DCC7E9F554D4F68A4E96`
to `1B802987E70E5074EA5D777F76BCF5C12A41290D58D172173D44005EB5D2B8BC`.
Read-only re-diff matches `diff-hunk-locations.json`: CALLF has three inserts
plus a 4-to-1 line stack-pointer replacement; LMSW has three inserts and no
replacement of the original CR0 masking/store. Both are selected CPU40 code,
so modified original files remain partial; equality of unmodified lines is
not a quantitative behavioral completion percentage.

## CALLF behaviors and evidence

- `call.c:90-120`, DIV-310, is a behavioral FastBOP adapter: when protected
  non-VM mode and a specifically reserved selector/one of three offsets match,
  it dispatches the original BOP, handling an inline byte or WOW/return
  special cases and bypassing normal far-call handling. When selector is zero
  or mismatched, `mvdm_softpc_fast_bop_redirect` returns zero and original
  CALLF branch continues. Registered at `src/mvdm/README.md:868`; its older
  entry expressly says unaccepted and lifecycle/callback closure unresolved.
  S2 CPU40 reconciliation documents why native monitor addresses cannot run
  as guest selectors and records only bounded fixture evidence. Do not claim
  complete FastWOW or all 16-bit callbacks from this source linkage.
- `call.c:122-131`, DIV-305, is a diagnostic witness before normal CALLF
  branch. `mvdm_softpc_report_wow_far_call_attempt` is enabled by both
  `MVDM_WOW_CALLBACK_CPU_TRACE_PATH` and a matching
  `MVDM_WOW_CALLBACK_CPU_SELECTOR`; it reads descriptor/caller bytes and
  appends a bounded record, preserving host last-error. It returns without
  taking this route when either environment variable is missing. The current
  README has no DIV-305 entry; record the full inputs, guest read bounds,
  failure/reentry behavior, environment opt-in and actual test evidence before
  any decision to keep/remove it. Its inline comment alone does not establish
  that enabled diagnostics cannot affect every failure or timing path.
- `call.c:252` replaces original operand-width-selected SP/ESP assignment
  with `set_current_SP(new_sp)` after loading destination SS. README:676 and
  T430 S2 register this as DIV-327, sourced from an existing SoftPC fix;
  `tests/mvdm-host/ccpu_stack_transition_test.c` invokes production
  CALLF/RETF/IRET and existing T430 evidence states 36 stack/fault cases.
  This is real entrypoint coverage, not a newly run test or a claim of complete
  guest decoder integration. The repair is present without an inline DIV-327
  marker at that CALLF site.

## LMSW witness and DIV-327 collision

`lmsw.c:52-65` adds before/after calls around the original CR0 masking and
`SET_CR` without changing their source statements. The adapter checks
`MVDM_WIN31_SETUP_MODE_TRACE_PATH`, emits at most 64 records per thread and
restores last-error; when unset it emits no records. This is a diagnostic
addition, not the stack-pointer fix. Inline it is also labelled DIV-327.
Separately `src/mvdm/README.md:1408` assigns DIV-327 to COMMAND first-WOW
classification and lists `cmdexec.c`, not the LMSW call site. README:676
assigns the same identifier to the distinct CALLF/RETF/IRET stack fix.
Consequently these three source behaviors cannot be resolved by treating the
number as a unique authority. Preserve each existing behavior and its evidence;
reconcile the registration by source location and cohort first, then assign
unique identifiers and cross-references as an explicitly reviewed metadata
change. Do not simply renumber blindly or silently delete a witness. No source
or registration was changed by this read-only audit.

Selected-build status follows the frozen graph; dynamic statement execution,
full FastBOP failure/return lifecycle and witness behavior under diagnostic
activation remain bounded by the cited historical tests. Neither selected
object nor the retained witnesses imply full original opcode equivalence.
