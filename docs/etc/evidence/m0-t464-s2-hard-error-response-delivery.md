# M0 T464 S2 — WOW hard-error response delivery

## Kept ownership boundary

The selected original producer remains
`softpc.new/host/src/nt_error.c::WOWpSysErrorBox`: it builds the exact
four-parameter `STATUS_VDM_HARD_ERROR | 0x10000000` request, retains the timer
bracket, and maps the returned source slot in its original callers.  The
selected original `wudlg.c`, `wuser.c`, and `wow32.c` callers remain the only
owners of guest return, task exit, debugger, and hung-app cleanup.

Modern public USER cannot compose NT4 `DisplayVDMHardError`: that server owns
private CSRSS/USER desktop attachment and message resources.  The project
therefore retains one existing narrow receiver rather than importing a private
server or introducing a second terminator.

## Delivered receiver contract

`mvdm_wow_hard_error_decode_buttons` is a pure decoder below the existing
receiver.  It accepts only source styles 1--8, omits empty/unknown slots,
compacts visible buttons, preserves each original one-based source slot as its
result identity, selects the first visible slot by default, lets every later
valid `SEB_DEFBUTTON` replace that default, and records the source `Cancel`
slot.  The public modal surface returns that cancel source slot for `WM_CLOSE`
or `IDCANCEL`; without a source Cancel slot it retains the previous no-response
close policy.  No host `IDOK`/`IDCANCEL` value crosses the original caller
boundary.

This is intentionally not a second dialog manager.  It does not modify guest
code, recreate private USER/CSRSS, change the original timer bracket, or own
task/worker termination.  Original resource-localized labels and the exact NT4
server `CancelId`/title-bar behavior beyond an explicit `SEB_CANCEL` remain the
documented private-server boundary.

## Evidence

`tests/observation/verify-wow-hard-error-decoder.ps1` builds the retained x86
decoder fixture and passes five assertions:

```text
PASS sparse count=2 default=3 cancel=1
PASS later-default count=3 default=2 cancel=2
PASS invalid-omitted count=2 default=3 cancel=0
PASS implicit-first-default count=2 default=2 cancel=0
PASS source-slot/default/cancel decoder contract
```

The final run is `build/t464-s2/hard-error-decoder-r10/`; its input hashes are
retained there.  The private-desktop attempt to automate an actual modal
response was rejected by the host (`SetWindowsHookEx(WH_CBT)` returned
`ERROR_INVALID_PARAMETER` for the isolated thread).  It was removed rather
than replaced with polling, a hidden helper window, or UI automation.  Thus the
decoder and selected-caller mapping are proven here; real interactive modal
presentation is explicitly retained for S4.

The real x86 worker graph was regenerated with the repository cache paths and
built through `ntvdm.exe`; a follow-up Ninja request reported `no work to do`.
The coherent ten-image candidate at `build/t464-s2/package-r4/` changes only
`ntvdm.exe` to SHA-256
`987FAE483E45010B233E00910E223A052DE416EB226555FEC8AD63D9E7D719FA`.
The remaining nine manifest identities equal the preceding release.  The
documentation governance gate passed.  Guest media was neither read as an
input nor modified.

## S3 handoff

S3 audits only the pre-existing original fatal tails after a returned slot:
`W32HungAppNotifyThread`, `RET_FORCETASKEXIT`, debugger continuation, and
`KillTask`.  It must prove normal, failed, and task/worker-loss cleanup has one
owner; it may not add an adapter terminator or expand this receiver.
