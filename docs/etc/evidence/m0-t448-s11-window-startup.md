# T448 S11 — CAF and WOW startup investigation

Owner reports `run16 command` followed by CAF does not show its window, and
`run16 sol` shows neither WOWEXEC nor Solitaire. These are separate acceptance
targets; a failure before the COMMAND prompt does not prove a CAF failure.
No product or guest binary was changed or published during this investigation.

## Baseline and controls

All thirteen installed `O:/winnt/system32` release images match the release
manifest. The installed SOL and package SOL have SHA-256
`68B196C6FD496BAC0E2F30F162EE0B4B4D6EB5BFEC1AF086C8EA759222211F31`;
installed COMMAND and package COMMAND have SHA-256
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
The installed CONFIG.NT and AUTOEXEC.NT differ from the test package, including
owner Windows 1.01 SETVER configuration. They were neither changed nor
executed by the isolated-package controls.

On the same S9 release package, `caf` with a controlled PATH confirms COMMAND
prompt, CAF visible-window ownership, natural return and echo. `sol-r3` uses
a private desktop and verifies a visible SOL window, stock draw, Deal and
normal close. The SOL test manifest-array bug and the diagnostic builder's
handling of Ninja implicit dependencies were corrected in test tooling; neither
is a product repair. Both test outcomes are controls, not proof the owner's
actual reported paths are repaired.

## Inherited-environment reproduction

On the active Default desktop, the S9 package with the controller's inherited
PATH reproduced SOL startup failure twice: no Solitaire or WOWEXEC window,
and the actual NTVDM illegal-instruction dialog identified real-mode
`05d8:213b`, bytes `63 68 61 69 6e`. Controlled short PATH on the same desktop
showed both WOWExecClass and Solitaire windows. First-half and second-half
PATH controls separately showed both windows. An independently constructed
single 1000-character path element also caused illegal instruction; 500 and
250 character elements and thirty 50-character elements showed the window.
These are bounded controls, not a general maximum or an approved truncation.
Raw private test output is in `build/t448-s11-regression` until scratch
disposition; values of inherited environment variables are not recorded here.

The controlled COMMAND CAF test passed; with inherited PATH its test instead
failed **before the prompt**, so it did not reach CAF and does not explain the
owner's specific post-prompt/window observation. The test package shares the
product broker endpoint; whether the owner's earlier launches coincided with
the prior isolated testing is unknown. No contention is asserted as cause.

## Original guest ownership and exact observed write

The pre-existing [TODO](../../states/TODO.md) and
[T447 source investigation](m0-t447-s3-control-source-audit.md#follow-up-environment-writer-attribution-and-class-collection-repair)
already identify the same immutable COMMAND `EndInit` limit; this investigation
independently reproduced it with today's environment. The diagnostic-only
owner wrapper logged initial environment calls without recording their values:
DS=05d8, ES=0683, 016f paragraphs allocated; DS:203c (`EnvSiz`) lies inside
that allocation. Before the real environment copy `EnvSiz=016f`; immediately
after the copy it is `464f`, while the segment's MCB and initial stack remain
otherwise consistent in this bounded observation. The shipped binary's original
instructions at 03c7 and 03cf store and compare DS:203c around the environment
service; the subsequent failure branch reaches the original INIT error path.
Current source `rdata.asm` and its `command.map` match these instruction
locations. The earlier host DIV331 WOW-kernel environment capacity check is a
different, later boundary; the WOW command-delivery diagnostic was not reached.

This is an original guest lifetime defect in this tested route, not proof of
a host out-of-bounds write. The standing policy prohibits guest patches, live
guest memory patches and host environment filtering as a workaround. The known
limitation remains tracked; no product fix or S11 closure is claimed.

## Deferred S12 observations

Before the owner's S11 insertion, the S12 investigation established that the
published MEM prints 4294940784 as largest executable size in one controlled
direct run. A disposable INT33 probe changed mouse position, saved and restored
state, and exited 1 with `MOUSE-STATE-FAIL`; the source saver writes zero while
the restore side compares a computed sum. These are initial results, not a
complete S12 attribution or disposition. The attempted nested BAT witness
printed `Bad command or file name`; it is invalid for nonreproduction and no
TODO was removed. The initial wrong-root runs timed out and are likewise not
product verdicts. Original guest binaries were not modified.
