# S10 shard 2: WOW32 message, hook, dialog and EDIT conversion cohort

The original/current hashes for `wmdisp32.c`, `wowhooks.c`, `wudlg.c`, and
`wmsgem.c` were independently compared with the frozen OpenNT source. All
four are modified, selected product source files and remain partial. The
retained x86 link map lists `WM32Create` from wmdisp32.obj,
`ThunkEMMsg16`/`UnThunkEMMsg16` from wmsgem.obj, `W32DialogFunc` from
wudlg.obj and `ThunkMsgFilterHook`/`ThunkMsgFilterHook16` from wowhooks.obj.
This verifies selected object extraction and named entrypoints, not that all
message families or any particular guest callback executed.

- `wmdisp32.c` retains original message-specific native/guest conversion,
  callback and unthunk decisions. DIV-344 makes DDE thunk mode thread-private
  and conversion records call-owned. DIV-345/347/349 track partial object,
  string, stack and heap acquisition and unwind on failed or exceptional
  paths; the original converter's copyback/ownership ordering remains the
  behavior owner. `wow_message_lifetime.c` consumes captured keys before
  release and delegates to existing allocators/unthunkers, rather than
  registering a second message implementation. For CREATE/MDICREATE,
  COPYDATA, text and clipboard branches, each acquisition and exceptional
  transfer must be considered individually, including a guest that changes
  a pointer while nested.
- `wowhooks.c` retains original hook dispatch and per-hook structures while
  using call-owned message contexts and stack scopes. Message filters save and
  restore their thread-local DDE selector through nested finally paths;
  hook event and mouse callbacks likewise pair stack scope with callback
  completion. This does not imply all hook types or cross-worker global
  ordering are restored, nor may WM_DDE transport be equated to DDEML.
- `wudlg.c::W32DialogFunc` uses original dialog conversion/callback and
  call-owned cleanup in a finally scope; modal nested ownership is distinct
  from simply drawing a dialog. `wmsgem.c` rejects absent native allocation
  for EM_REPLACESEL/EM_SETTABSTOPS and pairs tab-stop cleanup with the same
  signed count condition. `FREEPSZPTR` in this WOW32 header is empty or only
  clears the local pointer depending on build mode; an early failed local
  allocation cannot be called a mapping leak solely from missing that macro.

## Evidence and bounded limits

T447 S4 P22 records 167 constructor controls, 72 negative assertions,
21 native capture transactions and package-level guest/native/modal witnesses
for its then-sealed artifact. It separately records bounded classic-DDE
original-body and selected guest message evidence, not general DDEML. Its
later idle-bit section explicitly describes an exceptional original-filter
witness that was then not yet proven; do not retroactively convert an older
pending statement into a present defect or success without checking its
subsequent owner. T457 S4 records a separate sealed package with eight guest
dialog phases and named hook creation/denial/nesting/exception checks; its
native input-desktop idle subcase is explicitly a non-pass. Neither package's
hash by itself identifies today's separately built artifact.

A concrete follow-on matrix should bind the current artifact and real guest
message owner to: normal, failed allocation and partial-conversion rollback;
nested message/hook reentry with destroyed/changed target; borrowed vs owned
COPYDATA/DDE payload release; task retirement or callback exception before
and after original copyback; original dialog result/error state; and EDIT
replace/tab-stop zero, negative and allocation-failure cases. Match each
constructor and destructor with observable once-only release, without
claiming every family from one dialog screenshot or fixture. No tests were
rerun and no guest or product code was changed in this audit.
