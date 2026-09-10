# M0 T389 S7 — INT 16 idle BOP and IRQ1 owner audit

## Question

Does the reached `BOP 16h` marker identify the second `COMMAND.COM` prompt
waiting for a character, and may its missing port-60 drain justify a keyboard
or CPU repair?

## Inputs

* Selected original source:
  `O:\repos.external\OpenNT\base\mvdm\softpc.new\base\keymouse\keybd_io.c`.
* Selected original firmware source:
  `O:\repos.external\OpenNT\base\mvdm\softpc.new\bios\spckbd.asm` and
  `...\roms\bios4.asm`.
* Current mirror counterparts below `src/mvdm-host/softpc.new/`.
* Fixed S6 declared-`command.com` observation:
  `O:\ntvdm64\observation-t389-s6-command-com-status-ver-r2.txt` and its
  sidecar reports, with the staged product SHA recorded in that report.

## Procedure

1. Compared the original and current `keyboard_io` NTVDM branch, the BIOS BOP
   table, and the original 16-bit keyboard firmware control flow.
2. Classified the reached marker and the deferred IRQ source offsets against
   `src/mvdm-guest/dos/v86/doskrnl/dos/NTDOS.MAP` without treating a relocated
   map offset as a definitive runtime symbol address.
3. Reconciled the observation's accepted-key/ICA-request records with the
   original IRQ1 and port-60 path.

## Observations

* Under `#ifdef NTVDM`, original `keybd_io.c::keyboard_io` is deliberately
  narrow: `AH=0` calls `WaitIfIdle`, `AH=1` calls `IDLE_poll`, `AH=2` calls
  `IDLE_waitio`, and `AH=3` updates keyboard lights.  It neither dequeues a
  key nor reads port `60h`.
* The original `spckbd.asm::int16h_vector` uses `BOP 16h` for idle accounting.
  Its polling and wait paths explicitly issue `AH=1` and `AH=2` respectively.
  Thus an `AH=1` BOP is not a proof that the final child shell prompt is
  waiting for normal DOS `CON` input.
* The same original firmware defines the distinct hardware sequence:
  `int09h_vector -> BOP 09h -> in al,60h -> BOP FEh`.  The hardware controller
  path from `keyba.c` is therefore expected to reach `ica_hw_interrupt`, then
  CPU40 acknowledgement, IRQ1, and only then the source-owned port-60 read.
* The fixed observation proves only the prefix of that hardware path:
  `MVDM-KBD-OFFER accepted=1`, `MVDM-KBD-GATE ... full=1`, and
  `MVDM-KBD-ICA-REQUEST irr=03 isr=00 imr=18`.  It has no port-60 read.
* The observation's post-`54:04` deferred CPU samples classify to ordinary
  NTDOS startup/file work (`GetCharType+0d`, `DEV001E+7a`, `$READ+83`,
  `PathSepGotCh+7`, `DEVNAME+3b`, and `GetCDSFromDrv+5e`) rather than a proved
  second-shell prompt frame.  This is classification evidence only; runtime
  relocation remains unproved.
* The selected `NTDOS.SYS` also confirms the important `$READ` classification
  without inventing a host interpretation: the byte sequence at mapped
  `$READ+83` is the original `SVC_DEMFASTREAD` return branch.  It tests carry
  after `BOP 50:42` and, only on carry, enters the original `BOP 50:16`
  normal-read fallback.  The sampled `IF=0` location is that branch, not a
  keyboard instruction or an interrupt-frame return.

## Interpretation and confidence

High confidence: the current observer's `MVDM-COMMAND-INPUT-READY` name is
too strong.  It is an original BIOS idle-status marker, not a prompt-ready
contract.  Its first injected key reached the unchanged Console/8042/ICA
prefix, but the experiment did not establish that the guest had reached the
ordinary child `COMMAND.COM` input loop.  The absent port-60 record therefore
does **not** support forcing IF, synthesizing IRQ1, reading port 60 from the
host, or writing a guest DOS buffer.

The original source also shows no missing BaseSrv/CSRSS keyboard reader at
this boundary: BOP `16h` is already owned by the selected SoftPC/BIOS source.
The first unresolved prerequisite is earlier source-owned NTDOS/CPU40
continuity after the successful `54:04` return and before the child shell's
actual DOS `CON` wait.

## Follow-up

Keep the keyboard implementation unchanged.  The next S7 evidence must
identify the source continuation that performs the second `COMMAND.COM` load
and establish the relevant CPU40 flag/frame transition before a key is sent.
The observation tool may later receive a more specific, source-proven prompt
gate; it must not convert host Console lines into BOP records or guest-memory
writes.
