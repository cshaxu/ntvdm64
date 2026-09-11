# M0 T404 S3 P12 native Console mouse interaction audit 001

## Purpose

Separate the reported apparent held-button behaviour from coordinate and
Console-presentation hypotheses before admitting another runtime repair.

## Baseline and publication

The public product was restored after every diagnostic run from
`build/M0-T404/S3/r008-p7-mouse-guest-attribution/original-softpc-process.exe`.
The resulting `O:\ntvdm64\ntvdm32.exe` SHA-256 is
`3E213BEE9F58F5CCE6E8F755056FE4A2909872E00EAC05A671A5036850D06CCB`.
Diagnostics used a temporary r009 image only while no worker was active; the
baseline was restored in a `finally` path.

After the accepted repair, the required staging tool published the formal x86
link to `build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe`. Both have
SHA-256 `3dc9d7f0e8a7836653dca50cf5d32ddacc316e85e4af14b9546a94051498b3aa`.

## Source route and unchanged callback ABI

The relevant original route is `nt_event.c` mouse-record consumption,
`nt_mouse.c` `host_os_mouse_pointer`, then `mouse_io.c` `mouse_int1` and
`jump_to_user_subroutine`.  The imported `mouse_io.c` callback setup remains
the recovered OpenNT source: `AX` receives condition mask, `BX` current button
state, `CX/DX` absolute position, and `SI/DI` accumulated motion.

The temporary callback-register and byte observations used for this audit
were default-off and did not alter a register, event, queue, pointer position,
or presentation. They were removed after the repair was proven; the retained
production change is the single CPU40 entry selection below.

## Automatic controls

The observer uses the public `CONIN$` queue and waits for the original mouse
callback registration.  Before exercising EDIT it waits for the source-owned
text plane to show `Untitled`; this prevents startup-time keyboard races.  It
then supplies an ordinary Escape make/break pair and confirms the original
8042 break marker (`MVDM-KBD-PORT60 value=81`).

The no-mouse Escape control showed the initial dialog followed by the normal
EDITOR status line, proving that Escape had cleared the welcome dialog before
the mouse experiment.  Its text-plane report is
`O:\ntvdm64\logs\t404-p12-escape-control-text-plane.txt`.

The document-area movement control then supplied exactly one public record:

```text
MOUSE_MOVED, x=26, y=11, dwButtonState=0
```

The original route reported:

```text
stage 1: x=26 y=11 buttons=0 flags=1
stage 2: x=26 y=11 buttons=0 flags=1
stage 4: x=208 y=88 call-mask=0x0001 motion=0,0
stage 9: AX=0x0001 BX=0x0000 CX=0x00D0 DX=0x0058
```

After that single zero-button movement, the source-owned text plane rendered
`HELP: Survival Guide`.  The evidence is
`O:\ntvdm64\logs\t404-p12-center-motion-{chain,text-plane}.txt`.

## Finding

The report is reproducible after the welcome dialog is actually dismissed.
It is **not** explained by a stale press, a missing release, or the host
rewriting a movement record into a button-down callback: both public record
and original guest callback register `BX` are zero.

The remaining failure boundary is downstream of the source-preserved callback
handoff: either the guest callback's execution/return semantics or its
interpretation of the otherwise correctly delivered movement.  Coordinate
association remains a distinct pending question, but no unproven transform or
movement filter is justified by this result.

## Callback-entry audit

The selected CCPU40 host header retains upstream
`HOST_BOP_IP_FUDGE = -2`.  The original mouse source therefore writes the
registered callback offset minus two to `IP` in the non-CPU30 branch.  P12's
default-off byte observation ran after the callback had been registered by
EDIT and reported:

```text
registered callback: 371A:0223
CCPU entry selected: 371A:0221
bytes at selected entry: 02 00 06 1E
```

`0223` begins with `06 1E` (`PUSH ES; PUSH DS`), a plausible 16-bit callback
prologue.  The preceding `02 00` decodes as `ADD AL,[BX+SI]`; it is not a BOP
marker.  The selected CCPU40 `c_cpu_simulate` starts `ccpu(FALSE)` from the
live EIP and its `SETUP_HOST_IP` fetch is based directly on `GET_EIP()`.
Consequently this is a strong, source-and-runtime-supported candidate for an
extra instruction executing before the registered callback.  It directly
explains how the callback could receive the correct handoff registers at the
host boundary but enter guest code with `AL`/flags already altered.

The direct `mouse_set_subroutine` audit rules out an earlier registration-side
adjustment: it copies the guest-provided `ES:DX` offset verbatim to
`user_subroutine_segment:user_subroutine_offset`; function 20 delegates to
the same routine.  The runtime `0223` is therefore the registered handler
entry, not an already-fudged BOP address.

The owner then expressly admitted the single-point repair: “ok 那你实施吧”.
`jump_to_user_subroutine` now selects the registered `offset` only under
`CPU_40_STYLE` (the existing CPU30 direct-entry form already does so). The
global `HOST_BOP_IP_FUDGE`, CCPU, CPU30 and every other BOP caller remain
unchanged.

## Repair verification

The focused x86 build succeeded for
`build/M0-T404/S3/r009-p12-console-motion-vector/original-softpc-process.exe`.
The post-repair entry witness selected the registered target and saw its first
four bytes rather than the two preceding instructions:

```text
selected entry: 0223; registered entry: 0223
bytes: 06 1E FA 50
```

After the same UI-ready and Escape control, one no-button document-area move
preserved the original callback ABI (`AX=1`, `BX=0`, `CX=0x00D0`,
`DX=0x0058`) and the source-owned text plane stayed on `Untitled`; it no
longer rendered `HELP: Survival Guide`. The bounded record is
`O:\ntvdm64\logs\t404-p12-direct-entry-motion-{observer,chain,text}.txt`.

A second automatic control supplied press, drag, and release. Its unchanged
guest callback values were respectively `AX/BX = 2/1`, `1/1`, and `4/0`, at
the corresponding absolute positions. The text plane stayed in the normal
EDIT document view, with no `HELP:` record. The bounded record is
`O:\ntvdm64\logs\t404-p12-direct-entry-sequence-{observer,chain,text}.txt`.

Each diagnostic run restored the P7 public product in `finally`; its SHA-256
remained the baseline value above. The automatic controls established the
narrow entry repair before formal staging.

## Disposition

The owner accepted the staged product in ordinary `cmd`/`conhost` with
`EDIT.COM` on 2026-09-11: “太酷了！成功了！”. P12 is delivered. No mouse
policy, coordinate transform, presentation behaviour, or CPU30 path was
admitted. The retained conclusion is limited to CPU40 direct INT33 callback
entry; whether any historical BOP trampoline should be recovered is a separate
architecture/admission question.
