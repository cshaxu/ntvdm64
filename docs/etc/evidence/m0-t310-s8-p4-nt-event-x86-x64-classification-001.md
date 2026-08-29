# M0 T310 S8 P4: nt_event x86/x64 Classification

## Question

Which tracked event-manager warnings are live x86/x64 ABI defects after the
current modern NT I/O adapter is selected?

## Procedure

- Read the event loop, fixed keyboard-history buffer, keyboard LED NT I/O
  route, and BIOS-key INPUT_RECORD conversion.
- Force formal Ninja rebuilds of obj/host/nt_event.obj on x64 and x86.

## Result

- The old IO_STATUS_BLOCK incompatibility no longer emits: the same-shaped
  OPENNT_IO_STATUS_BLOCK adapter is selected.
- key_history_tail minus key_history is a private index in a fixed 100-entry
  array. The narrowed result is provably bounded and never transports a host
  pointer or guest identity.
- The remaining l-value union-byte cast is a source-style warning, not an
  architecture-width transition. It remains visible without suppression.
- No source or overlay change is justified by the current x86/x64 evidence.
