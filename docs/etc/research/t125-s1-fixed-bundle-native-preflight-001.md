# T125 S1 fixed-bundle native preflight 001

## Question

Can the T124 fixed process boundary launch the retained T121 native engine
without passing runner/BYOB arguments to the Bochs parser, and what bounded
native execution category results?

## Inputs

- `artifacts/build/current/t124-bochs-engine/ntdos64-run.exe` and
  `ntdos64-bochs-engine.exe` from the focused T124 build;
- the T121 executable and T122 ROM inputs copied into
  `artifacts/analysis/t125-s1-fixed-bundle-preflight-001/bundle`;
- the manifest in that bundle, whose four input hashes are repeated in
  `history/m0-t125-closure-20260812.md`;
- T122's retained BYOB profile/root, used only as the already validated CLI
  profile input.

## Procedure

The runner invoked the shim with `--engine`, `--bochs`, profile/root and the
profile target. The shim's only native child arguments were `-q -f bochsrc`.
Standard output/error were redirected to the evidence directory. After 15
seconds an external watchdog used `taskkill /T` against the known runner PID;
the recorded process tree was the only terminated scope.

## Observations

`stderr.txt` records fixed-config parsing, no-GUI initialization, 4 MiB RAM,
the BIOS at `0xfffe0000`, deferred POST ROM at `0xca800`, and the declared
native reset-floor devices. It then records 7,295 copied BOP observations.
Every observed one is `50:11`, with `CS:EIP=0070:0475`, `DS=0070`, `SS:SP`
`0000:0700`, `BX=018b`, `SI=00fe`, and #UD vector target `f000:ff53`.

`result.txt` records `watchdog-terminated`; `watchdog.txt` identifies the
terminated child tree. No native exit result or guest completion occurred.

## Interpretation and confidence

High confidence: the process boundary and fixed bundle are accepted by native
Bochs and reach the guest's BOP ingress. High confidence: this concrete image
is in a `50:11` re-entry loop during the bounded interval. Low confidence on
cause: the trace alone cannot distinguish an incorrect existing result from a
missing native precondition. T126 must use the original source contract before
selecting any owner or change.

## Follow-up

Audit the exact historical DEM `50:11` dispatcher, caller and continuation;
compare its current source-derived provider disposition and the earlier
`50:36` observation. Do not add a handler or alter Bochs from this record.
