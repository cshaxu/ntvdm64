# M0 T324 S2 — Original local Redirector binding recovery plan

## Objective

Recover one complete, local-only VDMREDIR cohort through original source:
`vrdisp.c`, `vrmisc.c`, `vrputil.c`, `vrmslot.c`, `vrnmpipe.c`, the reached
`cmdredir.c` caller and selected `ntstatus.c`. The work preserves original
dispatch, request records, register outcomes, worker ordering and failure
direction; adapters may bind only the historical external interface shape.

## Admission basis

T324/S1 established that these units form a finite cohort. Mailslot and
handle identities already use the session's sole host-resource mapping;
named-pipe asynchronous completion still requires an explicit SoftPC/ICA
completion transaction. No other VDMREDIR body may be enabled by this S.

## Required recovery ladder

1. Select directly composable original source and public Win32 calls first.
2. Reuse the existing same-shaped `adapter-mvdm-host-out/redir`, SoftPC and
   session bindings where they preserve the source call and failure shape.
3. If the original async callback cannot compose directly, record the exact
   required completion transaction before writing a bounded adapter body.
4. Do not create a second Redirector provider, new mapping manager, raw
   pointer/handle ABI or synthetic device/network success.

## Explicit exclusions

- `vrnetapi.c` calls through `XsNet*`, `vrremote.c`/RAP, `vrnetb.c`, and all
  `vrdlc*` execution remain the source-shaped later VDD/network/DLC owner
  transfer.
- `VDDInstallUserHook`, `VrRaiseInterrupt`, and hardware completion cannot
  become a local Win32-only success path.
- This S does not choose a BOP leaf from the trace or change guest media.

## Exit evidence

- One declaration/build selection map proves each local source unit's include,
  callback, handle and guest-span binding.
- One source-shaped completion-contract record states pending record ownership,
  session binding, mapping-lease epoch, resume/interrupt conditions,
  cancellation and teardown behavior.
- Original source selection compiles through the declared bindings or records
  an exact later-owner transfer; no warnings are hidden and no historical
  source body is replaced.
- Focused local positive/negative checks cover stale identity, failed public
  API, cancellation and absent device/RAP paths; formal selected x86/x64 graph
  review, governance gate and `git diff --check` complete the packet.
