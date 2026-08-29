# M0 T310 S8 P4: Event / CCPU Thread Declaration Closure

## Source disposition

- `PointerAttachedWindowed` is defined as original SoftPC `BOOL` state in
  `nt_event.c`; its header had relied on NT4 default-int inference.
- `key_history_count` is a private original keyboard-event count, likewise
  missing only its `int` spelling.
- `nt_thred.c` calls existing CCPU providers `ccpu386newthread` and
  `ccpu386exitthread`. The original `ntthread.h` also carries executor-private
  `jmp_buf` declarations that the host thread wrapper neither reads nor owns.

## Minimal recovery

The header/source now state the actual selected scalar types. The thread
wrapper declares only the two original `void` CCPU contracts it calls, keeping
the original thread creation, lifetime and exit sequence unchanged. No mapping
manager, guest state, session policy or controller algorithm was added.

## Verification

Formal Ninja rebuilt `obj/host/nt_event.obj` and `obj/host/nt_thred.obj` on
x64 and x86 with exit code zero and without C4431/C4013 for these four
contracts.
