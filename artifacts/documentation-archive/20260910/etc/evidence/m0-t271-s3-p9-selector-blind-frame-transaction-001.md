# M0 T271 S3 P9 — selector-blind copied-frame transaction

The temporary COMMAND ingress previously copied a generic `#UD` event into
CCPU-shaped state, invoked checked ordinary-RAM directly, and converted the
typed result into the generic machine outcome.  Those are machine-transition
mechanics, not COMMAND provider semantics.

`adapter-bop/frame_transaction.{h,c}` now owns that atomic borrowed
transaction:

- copied generic event to `runtime_exception_event` and `runtime_cpu_state`;
- a source-body-visible `runtime_cpu_result`;
- checked ordinary-RAM read/write callbacks; and
- typed resume/pending/controlled-stop outcome conversion.

The record has no selector, service, provider, DOS, WOW, COMMAND or session
field.  `opennt-bop/ingress/command_native_session.c` retains the OpenNT
entry-specific work: it recognizes the `MS_bop_4` envelope, stages the
original service byte, advances the staged source IP, and calls the original
COMMAND body.  Thus this extraction does not move selector interpretation
into `adapter-bop`.

Verification was performed outside the sandbox using formal Ninja root
`build/M0-T271-S3/r003`:

- `t271-s3-frame-transaction-fixture.exe` exited zero.  It proves copied
  exception/CPU fields, pass-through decline, typed resume/GPR/CF conversion,
  and vector rejection without any provider input.
- `t231-s10-command-native-session-fixture.exe` exited zero and reported
  `T231 v2 COMMAND session owns 54:00 without a v1 fallback`; it covers the
  modified copied-event COMMAND ingress path.
- `t236-s1-command-local-child-fixture.exe` exited zero after the formal
  relink, retaining the S3 P8 child lifecycle contract.

The retained `opennt_command_composition.c` still contains source-shaped
COMMAND provider glue.  P9 closes only the generic copied-frame portion; a
later S3 packet must either rehome each remaining non-mirror block to its
named adapter/host/session owner or prove it inseparable from a minimal
original source body.
