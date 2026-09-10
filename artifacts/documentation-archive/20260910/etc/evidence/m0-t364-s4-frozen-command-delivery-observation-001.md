# M0 T364 S4 — Frozen COMMAND delivery observation

## Question

With the unchanged CPU40/x86 product and package container, does the selected
original `/C EXIT` command reach and return through the original COMMAND
delivery path, and what is its first source-owned successor?

## Fixed invocation

```text
node tools/observation/ObserveSoftpcStartup.mjs \
  --launcher build/M0-T356/S4/observation/console_startup_observer.exe \
  --product build/M0-T362/S2/x86/original-softpc-process.exe \
  --stage O:\\ntvdm64 \
  --report O:\\ntvdm64\\observation-t364-s4.txt \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\\ntvdm64\\observation-t364-s4.txt.bop-return.txt
```

The observer used the fixed console-owning non-debug container, immutable
stage manifest `d58a396332401eedc0e482331f5bb1225290d7e398ba5dcf1370ad84b14abdaa`,
fixed media manifest `43f09b928f459fbca4c26a6cd6a24f08faf23e74c5ea3a7c291c955828de1cfb`,
and product hash
`e2a8d705159738b5900ea197b8d7aed64bbac178102e98d2954eabdcc01730f2`.

## Observation

The controlled eight-second observer ended with its defined timeout
`0x53504354`; it did not report a product exception.

The child report recorded the original setup route, then:

```text
MVDM-BOP-DISPATCH 54:01
MVDM-CMD-CONT svc=01 stage=0 ... first=1 repeat=0 dos-state=00000001
MVDM-CMD-CONT svc=01 stage=1 ... first=0 repeat=0 dos-state=00000002
MVDM-BOP-RETURN 54:01 ... ax=0002 cf=0
MVDM-BOP-DISPATCH 54:0E ... RETURN ... cf=1
MVDM-BOP-DISPATCH 54:04 ... RETURN ... cf=0
...
MVDM-BOP-DISPATCH 54:01
MVDM-CMD-CONT svc=01 stage=0 ... first=0 repeat=0 dos-state=00000002
```

No return marker follows that later `54:01` before the controlled timeout.

## Interpretation

The first selected application record was delivered by the original
`cmdGetNextCmd` route and returned without a fabricated BOP result.  The later
entry is exactly the source-owned ordinary no-next-DOS-record wait classified
by T364 S1/S2.  It is not evidence of a failed `54:01`, `54:04`, `54:0E`,
printer, SoftPC CPU, guest-pointer, or COMMAND built-in handler.

This run proves only the delivered bootstrap `/C EXIT` route and the expected
post-consumption wait.  It does not prove a DOS child, COM/MZ EXEC, native
child, interactive console producer, WOW, or a fully completed command
session.

## Transfer

T364 is closed.  The next owner package is original DOS `.COM`/MZ `.EXE`
execution and parent return.  Its first source audit must select an immutable
low-dependency DOS workload and follow original `$Exec`, JFN/SFT, PSP/arena,
environment and COMMAND return ownership; it must not treat the ordinary
post-bootstrap wait as a leaf COMMAND defect.
