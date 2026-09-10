# M0 T379 S5 — Frozen original REDIR observation

## Frozen invocation

Exactly one non-debug, console-owning invocation was made:

```
console-startup-observer.exe O:\ntvdm64\ntvdm.exe O:\ntvdm64 \
  O:\ntvdm64\observation-t379-s5-redir-frozen.txt \
  -f -o --command REDIR.EXE
```

The format-4 stage manifest identifies the launched parent as `ntvdm.exe`,
SHA-256 `adbdfc3bf72fbf41798ab299ee64b489e35096b86727274552b4c30199a57e1f`,
and its sibling `VDMREDIR.DLL` companion as SHA-256
`74dfd1f242b2169f8bc589e677ad242196e8034c4404a4e9ae226b231ec52198`.
The observer's JSON seals manifest SHA-256
`91ade2abe857e876c97336e789370aadc8b614dcd8c783ce37c9288c78a6f011`,
media-manifest SHA-256
`59c6a2df219d022739312d44079354baf8cddd50f38b119fde502e50a3339004`,
and companion-manifest SHA-256
`da26752b739f52afa350dfd5bb2f5a116de47e4ee481e794657935d84b1dca0e`.

## Durable result

The observer report records `container=console-owning-nondebug`, an eight
second bound, `result=exited` and `exit=0x00000000`.  Its durable DEM-open
record proves that the selected original `O:\NTVDM64\SYSTEM32\REDIR.EXE`
was opened successfully through the already selected original DEM route.

The BOP-return record continues through the original boot/COMMAND sequence
(`50:11`, `50:3B`, `50:0F`, `50:1B`, `54:05`, `54:0C`, `54:01`, and their
normal preconditions).  It contains neither `MS_bop_7` nor a `57:xx` BOP.
Therefore this single run does **not** prove `LoadVdmRedir`, DLL export lookup,
`VrDispatch`, a named-pipe transaction, or general Redirector compatibility.

## Classification

The first unproved boundary remains the later original COMMAND/guest execution
continuation after the selected `REDIR.EXE` has been opened; the frozen run
does not reach the Redirector BOP ingress.  This is not a reason to add a
trace-selected Redirector provider or to rerun the workload.  S1--S4 already
close the original DLL product, parent identity, export/import surface and
local resource lifecycle; S5 closes only the one identity-proven selected
workload observation required by this package.
