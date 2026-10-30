# Direct attribution of long-environment WOW startup witness

An isolated diagnostic worker reused the existing host-only
`command_environment_trace.c` wrapper; the original guest image was not
changed. The tested COMMAND.COM SHA-256 is
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`,
the same image identified by the accepted original-guest issue in TODO.

For a synthetic long PATH, the second environment service entered with
DS=CS=05D8, environment segment 0683, MCB size 01A4 paragraphs, and
`EnvSiz` at DS:203C equal to 01A4. The MCB included that field. After the
original service, `EnvSiz` had become 3D52. The same run displayed an illegal
instruction at CS 05D8. The short PATH control used the same diagnostic
image; its second service had 0109 paragraphs, did not cover DS:203C, and
`EnvSiz` remained 0109 before and after; SOL's main window was visible in
four samples. Logs, manifests and hashes are in this directory.

This directly reproduces the same discarded-INIT `EnvSiz` overwrite described
in T448 S13 under the WOW startup workload, rather than merely correlating
PATH length. It supports attribution of this controlled long-environment
startup failure to that existing immutable guest issue; it does not establish
that all historic WOW errors share that cause, the exact failure threshold,
or every later instruction transition. The diagnostic worker is not a
release image. No guest rebuild, environment filtering or product workaround
was made; the owner already chose to retain this guest defect in TODO.

The diagnostic package and current candidate were compared by unsealed input
hash: nine of ten components match; only `ntvdm.exe` is the diagnostic worker.
The exact comparison is in `input-comparison.json`. Different package sealing
identities and the instrumented worker remain relevant; this does not make the
diagnostic execution a product acceptance run.
