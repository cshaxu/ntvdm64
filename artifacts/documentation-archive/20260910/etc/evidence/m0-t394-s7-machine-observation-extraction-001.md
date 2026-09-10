# M0 T394 S7 — machine observation extraction

Date: 2026-09-06

CPU, keyboard/8042, PIC/ICA, stream and SAS/direct-memory default-off
observers were extracted from `mvdm_softpc_termination.c` into
`mvdm_softpc_machine_observation.c`. Their explicit declaration owner is
`mvdm_softpc_machine_observation.h`; all twelve reached original CCPU,
keyboard, ICA and host-path consumers now include it directly.

The original exception path retains only its selected flush timing and calls
the machine component's flush API. COMMAND, DEM, BOP, exception and main-return
observation bodies remain in the former aggregate unit for S8. No CPU, PIC,
keyboard, SAS, guest-memory, BOP or report behavior was changed.

Direct compilation of the machine unit passed x86/x64; the remaining aggregate
unit passed x64 after its declaration cleanup. The T310 source generator lists
both units, and `git diff --check` passed.
