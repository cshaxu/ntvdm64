# M0 T423 S17 — Window exit to outer CMD input recovery

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and inputs

Why can an outer `CMD` appear to stop echoing after `run16 command`, a Window
route (`Ctrl+Alt+F`), and DOS `exit`, although the command still accepts and
executes a blind `echo CHECK123`? The inputs are the published S16 package,
the immutable `O:/winnt` guest media, the ordinary frontend observer, and the
affected `run16-exe` and `ntcon-exe` source paths.

## Cause

The Window route makes NTCON select a temporary Console screen buffer. Its
teardown restores the outer CMD's original buffer and input mode in
`run16_native_frontend_destroy`. Before this S, the root launcher returned to
the outer CMD after its DOS record completed, with no acknowledgement that
this restore had finished. The outer shell could therefore enter its next
line-input cycle during the active-buffer/input restoration transition.

This is a host frontend lifecycle race, not a DOS/COMMAND failure: the
owner's process inspection already showed no remaining product process, line
input and echo enabled, no selection, and the original buffer selected.

## Repair and ownership

The repair adds two private inherited synchronization events to the existing
root `run16` → NTCON bootstrap; it changes no guest code or OpenNT/MVDM
semantics.

| Record | Owner | Meaning |
| --- | --- | --- |
| `retire` | root `run16` signals; NTCON observes | The root direct DOS completion is now eligible for frontend retirement. It prevents the early-start interval (frontend registered before the DOS record) from looking globally idle. |
| `restored` | NTCON signals; root `run16` observes | Original active buffer and input mode have been restored, or no acknowledgement is sent on restore failure. |

`run16` signals `retire` only after the direct DOS completion and original
parent-resume work. It then waits for `restored` before it returns to its
outer CMD. NTCON keeps its broker-reported pending/task/member check: another
legitimate frontend user is not torn down merely because this root completed.
The retire event is latched after first observation, so a still-live frontend
does not spin on a signaled manual-reset event.

`frontend_service_close` now returns the native-Console restoration result;
NTCON signals `restored` only after that result is successful. A failed
restore returns a real error instead of falsely handing a half-restored
Console to the caller.

## Order witness

The temporary low-perturbation witness was removed from production source
after collection. `O:/winnt/Logs2/t423-s17-{console,window}-empty-r8.trace`
records the required Window route order:

```text
run16 task-completed
run16 native-resume
run16 frontend-retire
ntcon pump usage
ntcon pump retired
ntcon pump drain
ntcon restore
ntcon ack
run16 frontend-restored
run16 worker
run16 exit
```

Thus the returning launcher is observably after restoration acknowledgement;
there is no sleep, forced repaint, synthetic keystroke, or output write.

## Verification

| Check | Result |
| --- | --- |
| `frontend-scope-lifetime-test.exe` | Passes all four ownership/retirement cases. |
| Ordinary isolated Console `empty` | Pass: `t423-s17-final-console-r2-empty.txt`. |
| Private-desktop Window → Console `empty` | Pass: `t423-s17-final-window-r2-empty.txt`. |
| Existing actual-output/exit matrix | Pass: `t423-s17-dos17-r1` completed all 17 ordinary rows, including COMMAND, MEM, EDIT, nested COMMAND and direct/nested native routes. |

The Window test uses the public Window input route but an isolated desktop;
it does not move or focus the owner's desktop. The ordinary Console control
does not take the Window route.

## Closure and follow-up

The owner directed S17 closure after reviewing the repair and its ordinary
production gates. The exact outer interactive CMD reproduction remains a
useful post-delivery observation, but is not represented as an unrun automated
pass. The unrelated retained 100 ms creator/`ERROR_BUSY` poll is registered
in [TODO](../../states/TODO.md); the S17 `retire → restore → restored` barrier
itself remains event-driven.

The coherent eight-file `O:/winnt` package at delivery carries these hashes:
`run16 E878015329D2173EDC942DBE4BBBF67FB39DDB08A5E034331DA9D77A56C811C3`,
`ntsrv 552FEEEA2115221B7C39461AD4F6E7B91249D032AF893CACE712F62DA02A48EF`,
`ntcon 453A7DF32259083AE389DCABE84D425E34DA6A59FD37886DF3359958D30C4C44`,
`ntvdm 338A9D2DEA003F1CB58D4F234E30442499FD3111279064B4FF88F750513C2BBE`,
`ntw32 64ACFC7C4F8AA9696FF25DB761C0AC9CF6AA84203A6E40A0417E2162B5B52570`,
`ntmon 589936026400EF4C4C6E5D212E53C5B0389DB42F8843238E392E5CBF78C2EBA3`,
`wow32 0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A`,
and `vdmredir 79698D12E90DC8F8ACF8FED250DC0A2C9BE57711C03E4DF44A0112549E023500`.
