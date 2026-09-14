# T412 S4 stream reservation binding

## Question

Can the standalone broker preserve the original `BaseSrvUpdateVDMEntry`
standard-stream copy, although that copy happens before the new worker has
connected to BaseSrv?

## Inputs

- Original `BaseSrvDupStandardHandles` / `BaseSrvCloseStandardHandles` in
  `srvvdm.c` retain the source order and stdout/stderr alias rule.
- Original BaseClient `BaseCheckVDM` supplies the three standard handles;
  the standalone copied Check record previously omitted them.
- The S4 three-program product and guest `COMMAND.COM` from `O:\ntvdm64`.

## Procedure

1. Preserve the original HANDLE-shaped fields only as broker-local receipt
   identifiers in the copied Check, Update and Get records.
2. Retain the typed file/pipe object in the authenticated launch reservation
   during `UpdateVDMEntry`; the reservation is released with its worker and
   drains retained objects.
3. On authenticated worker `GetNextVDMCommand`, resolve those identifiers and
   deliver the three objects through MIDL typed-handle output parameters.
4. Rebuild all three x86 products and run `COMMAND.COM /c run16.exe
   --internal-console-probe` with redirected parent standard streams.

## Observations

The first attempt deferred delivery until worker Get and failed at original
Update with `ERROR_INVALID_PARAMETER` (87).  A reservation-only first-command
variant then regressed the initial worker Get.  The currently executable path
therefore retains the original Update-time duplicate into the already
registered suspended worker.  The Get client accepts those already-local
handles without requiring a second RPC attachment.  A fresh real probe
returned 0 from the parent and recorded `Check`, `reserve`, `prepare`, worker
`Get`, reentry increment/decrement and parent exit-code with zero broker
status.  Its guest native-child trace recorded the copied payload,
standard-handle fields, guest capture locations and child exit value 5.
Logs are retained at `O:\ntvdm64\logs\m0-t412-s4-probe-worker-local`.

A second run using `COMMAND.COM /c run16.exe MEM.EXE` initially returned 31.
The cause was the typed Check attachment wrapper treating a `GetFileType`
unknown result with stale last-error state as an attachable resource.  It now
clears last error before the query and treats an unknown type with a set error
as absent.  The trace-free rerun reached a second original BaseSrv
Check/reserve/prepare/worker-Get sequence and the native child returned 0.
The redirected output files remain empty, so this does not prove
byte-producing guest redirection.

The direct CLI regression initially exposed a second boundary error: after
the original Update-time duplication had made the three standard handles local
to the suspended worker, the broker's subsequent Get tried to resolve their
numeric values as reservation receipts and returned `ERROR_INVALID_HANDLE`.
The reservation now records only the finite fact that a value was made local
by that same Update duplication. Get leaves those values untouched and sends
no second typed attachment. The x86 reservation test covers absent, marked and
duplicate local values. An isolated x86 relink staged `basesrv.exe` and
`ntvdm.exe`; `run16.exe` was unchanged. The live
`O:\ntvdm64\run16.exe O:\ntvdm64\MEM.EXE` direct-redirection observation
then recorded three successful Get calls, parent exit-code and disconnect at
`O:\ntvdm64\logs\m0-t412-s4-direct-mem-r2`. `MEM` writes its display through
the DOS Console path, so its empty redirected files are expected and do not
claim byte-producing output acceptance.

## Direct versus guest redirection

The staged guest case was separately run as `COMMAND.COM /c MEM.EXE >
O:\ntvdm64\logs\m0-t412-s4-command-flow-r1\guest.out`. COMMAND consumed
and returned from its initial command (the original continuation trace records
the service sequence), but no guest output file was created. That is the
current Redirector/guest-redirection unavailable boundary, not evidence that
the direct CLI file handles failed to transfer. The direct and guest cases are
therefore explicitly distinguished.

The third comparison passed literal `>` as an argument to `run16.exe MEM.EXE
> literal-target.txt`, without a host shell. It reached normal
Check/reserve/prepare/Get and exit at
`O:\ntvdm64\logs\m0-t412-s4-literal-redir-r1`; no host output target was
created. Thus run16 did not invent shell parsing for a literal operator.

## Interpretation

Confidence is high that the original Update-before-Connect ordering and
worker-local standard-handle consumption execute.  The receipt-only form is
not accepted: it remains an open design issue rather than a completed
transport claim.  Nested DOS execution, byte-producing stdout/stderr, EOF
and guest redirection remain required acceptance cases.

## Follow-up

Design a receipt-only first-command transfer that preserves the same ordering,
then exercise a byte-producing COMMAND redirection case before closing S4.
