# M0 T271 S3 P12 — selector-blind CCPU/SAS frame-context extraction

The retained COMMAND composition used to define copied CPU-register access,
typed result edits and single-byte `sas_load` locally.  Those are generic
CCPU/SAS mechanics, not OpenNT COMMAND provider semantics.

`adapter-softpc/ccpu_frame_context.{h,c}` now owns a thread-scoped borrowed
context over an already validated fixed-width CPU state/result pair and
bounded guest read/write callbacks.  It supplies the original-style register
and `sas_load` operations while recording changes solely in the typed delta.
It contains no selector, service, provider, DOS, WOW, COMMAND or session
field.  The imported source still uses its historical call forms through
private macros; only their mechanical backing moved.

This is registered as `SOFTPC-DIV-005`: the original process-global CCPU/SAS
backing is unavailable and unsafe in a bounded modern call, so the same
observable operation is implemented through a short-lived context.  The
context retains no caller pointer after `runtime_ccpu_frame_context_end`.

`GetVDMAddr` deliberately remains in the pending source-body review.  Its
allocation, guest writeback and service ordering are COMMAND-specific and
cannot be misclassified as selector-blind CCPU mechanics.  Likewise, P9's
checked span transaction remains in `adapter-bop`.

Verification was performed outside the sandbox after a formal Ninja relink
at `build/M0-T271-S3/r003`:

- `t271-s3-ccpu-frame-context-fixture.exe` exited zero.  It verifies the
  real-mode register reads, GPR/segment/CF typed deltas, IP update, bounded
  `sas_load`, pending disposition and end-of-scope rejection.
- `t231-s2-command-misc-direct-import-fixture.exe` exited zero and reported
  the original direct `cmdmisc` COMSPEC/current-directory ABI result.
- `t231-s10-command-native-session-fixture.exe` exited zero and reported
  `T231 v2 COMMAND session owns 54:00 without a v1 fallback`.
