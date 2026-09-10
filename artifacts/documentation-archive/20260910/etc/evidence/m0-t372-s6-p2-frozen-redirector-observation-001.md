# M0 T372 S6 P2 — Frozen Redirector observation

## Question

After the original whole-family dynamic-load disposition is frozen, what does
one unchanged, non-debug fixed-container execution of an original guest
workload establish about Redirector reachability?

## Fixed inputs

The one permitted observation used the already-built console-owning observer
and the existing immutable `O:\ntvdm64` media/firmware stage:

```text
O:\.nvm\versions\node\v22.22.1\bin\node.exe \
  tools\observation\ObserveSoftpcStartup.mjs \
  --launcher build\M0-T369\S2\observer\console-startup-observer.exe \
  --product build\M0-T371\S2\formal-x86\original-softpc-process.exe \
  --stage O:\ntvdm64 \
  --report O:\ntvdm64\observation-t372-s6.txt \
  --product-command LOADFIX.COM \
  --child-environment MVDM_COMMAND_CONTINUATION_REPORT_PATH=O:\ntvdm64\observation-t372-s6.txt.bop-return.txt
```

The launcher is the existing `console-owning-nondebug` observer, SHA-256
`dabaf70d12a17d64ac319211a40d3cdb482d2c236aed32eb50a3ae3aad20826d`.
The formal CPU40/x86 product copied into the stage is SHA-256
`7b6011aaa477eff1e00a541e08fc6e0a6f294fef1cbdf67e91cdf8fdeff5d457`.
The unchanged stage manifest is SHA-256
`5af5a7c9d9eecaeceadc325da899fd2d276fbea98557c20f5df193df23bf1077`;
its immutable media-only manifest hash is
`1ddbc5ef287194fdbf4c3074081a8bd019cf46d881c3823fb5fd24f0331d0fa9`.

`ObserveSoftpcStartup.mjs` validates every staged media asset against that
manifest before it copies the one allowed mutable product member. The launcher
returned zero; its product stayed confined to `O:\ntvdm64` and no guest asset,
firmware input, observer, command string, timeout, console ownership or
environment mechanism was changed.

## Observation

The primary report records:

```text
container=console-owning-nondebug
result=timeout
exit=0x53504354
timeout-ms=8000
fixed-system-root=O:\ntvdm64
image-path=O:\ntvdm64\original-softpc-process.exe
```

The source-return sidecar reaches the already-known original paths
`50:01`, `50:02`, `50:12`, `50:36`, `50:38`, `50:3C`, `50:42`, `54:01`,
`54:04` and `54:0E`. Its final recorded source event is another original
`54:01` stage-zero COMMAND continuation:

```text
MVDM-BOP-DISPATCH 54:01
MVDM-CMD-CALL svc=01 stage=0 ax=000E cf=0
MVDM-CMD-CONT svc=01 stage=0 cs=95EB ip=03C4 ax=000E bx=0001 cf=0 first=0 repeat=0 dos-state=00000002
```

There is no return before the prescribed timeout. Searches of the primary,
console and BOP-return sidecars find no `57:xx`, `VDMREDIR`, `VrDispatch`, or
`ERROR_INVALID_FUNCTION` marker.

## Interpretation

This is one valid, fixed-container **non-reachability observation** for the
Redirector family. It proves neither successful Redirector behavior nor the
original `MS_bop_7` dynamic failure at runtime, because the selected original
`LOADFIX.COM` path did not reach a `57:xx` ingress before the earlier
COMMAND/Base VDM continuation timed out.

S6/P1 remains the evidence for the selected profile's original dynamic result:
if a `57:xx` selector is reached later, the absent `VDMREDIR.dll` causes the
parent's first-attempt `CF=1`, `AX=ERROR_INVALID_FUNCTION`, `IP+1` path and
then its no-retry state. This observation does not alter that conclusion and
does not authorize a trace-selected provider patch.

## Raw evidence

The one-run report and its sidecars remain at
`O:\ntvdm64\observation-t372-s6.txt*`. They are external runtime evidence,
not source, build, link, discovery or acceptance inputs.

## S6 closure

Together with S6/P1, this satisfies the active packet's required distinction:

1. all 50 original `57:00..31` slots and ten original exports have one
   source-first selected-profile disposition;
2. static archive composition is not misreported as a dynamically available
   DLL; and
3. exactly one immutable observation is retained without claiming unobserved
   Redirector, network, COMMAND child, DOS child or WOW behavior.
