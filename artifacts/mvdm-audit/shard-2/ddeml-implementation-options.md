# S10 shard 2: DDEML route and public callback feasibility

## What is actually available

Read-only on 2026-10-26: `assets/winnt.zip` has no DDEML.DLL, nor do the
regular O:/winnt, O:/winnt/system32 or O:/winnt/system paths. None of the
62 archive-manifest NE imports names DDEML. This proves neither all apps lack
dynamic DDEML loads nor that the original feature is unavailable everywhere.

`O:/winnt/tests/w31p/SYSTEM/DDEML.DLL` is an immutable existing test-tree NE
module, 36,864 bytes, SHA-256
`6D86DE3A979AB3F4EEAC37EBFD8F43B33F7DC80E50EDE7112903E41DA77FC95B`;
`src/mvdm/wow16/ddeml/debug/DDEML.DLL` is another prebuilt NE, 71,168 bytes,
SHA-256 `4A8FAB7786A388882687E1A79D810534C94D0F9E05A313D22173C0C68C83A443`.
`tools/audit/audit-guest-archive.py::image` reports module DDEML and imports
KERNEL/GDI/USER/KEYBOARD for both. Matching module name/imports does not
prove code identity or source lineage. A specific application directory or
PATH may make a different copy reachable; that has not been observed.
The shard's `guest-loader-location-audit.md` records search-path uncertainty.

The existing `wow16/ddeml/ddeml.def` exports DDEINITIALIZE ordinal 2 and
`wow16/ddeml/ddeml.c::DdeInitialize` calls original 16-bit `Register`, which
owns guest instance structures and window/message handling. Its makefile
selects the many original guest manager modules. This source is a valid
architecture candidate, not proof that either prebuilt binary implements
that exact source or that the tested installed KERNEL supports all its
private dependencies. The guest must never be patched or rebuilt.

There is also an older original 32-bit thunk candidate: `wddetbl2.h` maps
ordinals to `WD32Dde*` in `wddeml32.c`. Current `wowtbl.c::aw32WOW` includes
KERNEL/USER/GDI/keyboard/sound/shell/Winsock/ToolHelp/multimedia/command
tables but not wddetbl2; the selected `wow32/sources` excludes wddeml32.
No existing importer or dispatch edge to that table was found in the selected
WOW32 source. Hence adding this table could be an unrelated alternative
provider rather than recovery of the actual guest's route. First prove the
real module/entry connection. `wdde.c`'s selected classic WM_DDE conversions
are a different layer; their witnesses do not prove DDEML instance APIs.

## Exact instance boundary and existing ownership

For the old candidate `WD32DdeInitialize`, original code captures the guest
ID and guest callback, calls native `DdeInitialize`, then binds returned ID
to guest callback in its `aCallBack[MAX_CONVS]`. Native
`W32DdemlCallBack` calls private `DdeGetCallbackInstance`, looks up that ID,
constructs `PARM16`, conditionally converts CONVCONTEXT and uses original
`CallBack16`. On successful `DdeUninitialize`, the old code unbinds the ID.
`aAccessData[MAX_CONVS]` separately owns access/unaccess associations.
Both arrays have 3,200 entries. The historical thunk also reacquires a VDM
frame after calls that can cause task switches. Its old success/error and
full-table paths require direct audit; they must not be assumed safe simply
because they are original.

Modern x86 USER32 on the inspected machine exports public `DdeInitializeA`
and `DdeUninitialize`, but not the private `DdeGetCallbackInstance` (version
and hash in `guest-loader-location-audit.md` companion
`ddeml-owner-boundary.md`). Microsoft documents the public callback function
signature without an instance argument and says different DDEML DLL
instances should have distinct callbacks. It also states that objects in an
instance are thread-bound and all operations must occur on its creating
thread. See [DdeInitializeA](https://learn.microsoft.com/en-us/windows/win32/api/ddeml/nf-ddeml-ddeinitializea),
[PFNCALLBACK](https://learn.microsoft.com/en-us/windows/win32/api/ddeml/nc-ddeml-pfncallback),
and [DDEML threading](https://learn.microsoft.com/en-us/windows/win32/dataxchg/basic-concepts).
Those APIs show a possible finite public seam; they do not prove a drop-in
replacement for NT4 semantics or that callbacks during initialization and
teardown have the same timing.

Original OpenNT `windows/core/ntuser/client/ddemlcli.c::InternalDdeInitialize`
is evidence for the hidden dependency: it allocates a client instance, sets
its callback and creating thread, and invokes `NtUserDdeInitialize` for the
server instance. Its private window, handle structures and USER/CSRSS
manager are not eligible for wholesale import; modern USER remains owner of
actual DDEML resources.

## Candidate design, conditional on proven need

**Preferred investigation: unchanged guest manager.** Stage an unchanged
prebuilt DDEML.DLL beside an independently authored Win16 test program in a
disposable isolated package, record both hashes and loaded module path, and
exercise guest DdeInitialize -> original guest Register -> classic DDE
message boundary with a real native peer. Retain the existing original guest
manager and classic WOW conversion; correct only a proved host boundary,
without introducing a second manager or changing guest. If this resolves
the actual guest's contract, the old WDD table remains a separate profile
question and does not need to be activated just because it exists.

**Alternative only if an eligible original guest actually dispatches the
thunk profile:** use public DDEML as host resource and retain original
`WD32Dde*` conversion logic. Replace the one missing private callback
instance query by a bounded per-instance callback association. The public
callback has no userdata, and multiple instances can share a thread, so a
thread-only variable or mapping by HCONV cannot cover all callback types
(e.g. a callback may lack a conversation handle). One candidate is a finite
set of statically compiled instance-specific CALLBACK entrypoints that enter
a common adapter with a slot index; map that slot to the corresponding
original `aCallBack` record and live task identity, not to an independently
maintained duplicate DDE manager. Preserve the original bound of 3,200 unless
an independently proven original contract says otherwise. This is a design
option, not a claim that such callbacks preserve every source semantic or
are acceptably small; map footprint, code generation and public API limits
must be measured. Dynamically generated executable code is not assumed.

Lifecycle under that candidate must be explicitly proven: bind guest callback
and task generation before any possible host reentry; on a first init reserve
a free slot before entering the public host; commit ID only on success; on
nonzero-ID reinitialization preserve the original callback/slot; enforce the
host-creating thread; reacquire original guest frame after callouts; on failure
roll back exactly once without exposing a stale ID; during uninitialize keep
context alive through any nested native callbacks, mark closing, and retire
the slot only after native uninitialize plus all active callback borrows
finish. Task exit must perform native instance cleanup on its original host
thread before retiring original callback context; worker shutdown must drain
that same path. Nested task/slot reuse and 3,200-slot exhaustion need explicit
original/result comparison, and unrelated WOW tasks must not receive callbacks.
No intercomponent control channel or independent scheduler is added.

## Required evidence before implementation/admission

1. Capture actual module load identity and search path for a test app, and
   whether that immutable module calls guest Register/classic DDE or reaches
   `WD32DdeInitialize`; verify file identity per run.
2. For guest-manager route, cover native-peer and Win16-peer connect,
   transaction/data/atom ownership, synchronous and asynchronous callbacks,
   callback reentry, failure, uninitialize, task loss and sibling survival.
3. For public-thunk candidate, first independently prove callback-instance
   association on x86 USER32 with two distinct instances on an admitted
   thread, nested callbacks, initialization/reinitialization and uninitialize;
   then compare original observable values/error/ordering and resource release.
   Check all callback types that lack conversation identity and any monitor
   mode, capacity and cross-task limits. User-facing observable semantic
   concession requires prior owner decision.
4. Record a source-lineage reason before claiming a guest module was built
   from the mirrored source, and use classic DDE tests only for their actual
   covered message/alias behavior. No current runtime result or feasibility
   pass is claimed by this note.
