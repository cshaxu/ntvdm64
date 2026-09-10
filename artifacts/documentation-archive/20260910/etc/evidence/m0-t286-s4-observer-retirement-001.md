# M0 T286 S4 — unconsumed observer retirement

T286/S1 established that no admitted product consumer uses the following
default-off copied observations. S4 removed their bodies, declarations,
call-sites, gates, manifest entries and formal diagnostic switches:

- segment access: `access32.cc` and `segment_access_hook.cc`;
- instruction history: `cpu.cc`, `instr.h` and
  `instruction_history_hook.cc`;
- software interrupt: `soft_int.cc` and `software_interrupt_hook.cc`;
- physical write: `cpu.h`, `memory.cc` and `physical_write_hook.cc`;
- interrupt return: `ctrl_xfer16.cc`, `ctrl_xfer32.cc` and
  `interrupt_return_hook.cc`;
- shared `observation_gates.h`.

The remaining opaque callback is used only by the fixed-width #UD event/result
contract. Real/V86 profile mechanics remain separate CPU behavior and were not
changed.

The post-removal source scan finds no observer macro, gate, hook method or
diagnostic definition in production sources or the formal graph generator.
The measured adopted-core baseline is now 29 differing files, 393 inserted / 356
deleted lines; the remaining 9 private overlay implementation files total 978
lines. `soft_int.cc` and `memory.cc` are byte-identical with pinned upstream.

Formal graph generation remains blocked by the independently recorded stale
pre-rebootstrap manifest paths; S4 therefore makes no compile-pass claim.
