# M0 T381 — `cmdExec32` predecessor transfer

T381 is not functionally closed.  Its active S2 completed P1's bounded
payload observation and P2's current-formal snapshot classification.

The one current-formal fixed observation never reaches `54:08`; it samples
the original CPU40 SoftPC LPT status path (`printer_.c:printer_io -> ios.c`)
at port `0x3BD`.  It was temporarily transferred to M0 T382 under the
pure-DOS-first rule.

T382's complete source/data-flow audit then proved that this is an ordinary
single-ready-status read, not a supported LPT blocking inference.  T381
therefore resumes immediately; no child-process, exit-37, or `54:08` runtime
completion is claimed by this history correction.

The resumed review preserves a separate earlier fact from the same frozen
observation: the current product has returned from `54:0F` but has not reached
the first `54:01`.  That is the distinct original COMMAND/Base VDM startup
delivery cohort.  T381 consequently transfers to T383; see
[the startup predecessor record](m0-t381-cmdexec32-startup-predecessor-transfer-20260902.md).
