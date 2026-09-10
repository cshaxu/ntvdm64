# M0 T355 S36 — source-shaped normal DOS command declaration recovery

## Question and scope

Can the selected one-session CPU40 product provide one explicit, normal DOS
command declaration to the already recovered BaseClient/BaseSrv boundary
without inventing a COMMAND result, an interactive console reader, or a CSR
server?

This recovery changes only the app-owned producer. It does not alter an
original MVDM COMMAND/DEM/XMS function body, guest image, firmware, BOP ABI or
the existing `50:42` FastRead disposition. `50:42` still performs its bounded
regular-file read when safe and otherwise returns `CF=1` so original DOS `$READ`
can take `50:16`.

## Source and boundary evidence

- `src/mvdm-host/dos/command/{cmddisp.c,cmdmisc.c,cmdexec.c,cmddata.c}` keeps
  the original `54:01 -> cmdGetNextCmd -> GetNextVDMCommand` caller route.
- The byte-identical OpenNT evidence mirrors remain
  `src/opennt-host/base/win32/{client/vdm.c,server/srvvdm.c}`. They establish
  capture/copy, DOS wait/retry and BaseSrv record-selection order.
- `src/adapter-mvdm-host-out/basesrv/source/{base_vdm_client.c,base_vdm_local.c,base_vdm_broker.c}` remains the selected source-shaped public replacement:
  copied input, session event, DOS-only pending/retry and copied record bridge.
  It does not claim CSRSS, CSR transport, separate WOW, PIF, cross-process
  broker, standard-handle duplication or child/parent EXEC semantics.

## Recovered declaration

`src/app/launch_declaration.c` now accepts exactly one explicit
`--command <text>` declaration. It rejects a missing, empty, duplicate,
CR-containing or LF-containing value. A valid value is copied into the
app-owned declaration and published through the existing broker as:

```text
application: <MVDM system root>\\system32\\COMMAND.COM
command:     /C <text>\r\n
environment: COMSPEC=<application>\0\0
directory:   <MVDM system root>
```

Thus the application is a deliberately bounded producer of an original-shaped
Base VDM DOS record; it does not execute the text itself or read ambient
console input.

`vdmapi.h` required one registered ABI-only mirror divergence:
`OPENNT-ABI-001` adds an include guard because the new broker composition
legitimately includes the historical declaration carrier twice in one modern
translation unit. The upstream declarations, names, layouts and API contract
are otherwise unchanged. The local fixture graph was also corrected to link
the broker and record objects that the app declaration already calls.

## Verification

1. A forced fresh x86 compile/link of the local Base VDM fixture, including
   the changed app declaration, broker and record source, completed:

   ```text
   PASS: local Base VDM broker contract
   fixture-exit=0
   ```

   The fixture verifies the positive `--command EXIT` record as
   `/C EXIT\r\n`, plus missing, empty, CR/LF and duplicate declaration
   rejection. It retains the pre-existing DOS pending/wake/retry, capacity,
   re-entry and DOS/WOW separation checks.

2. A freshly generated formal CPU40/x86 Ninja graph in
   `build/M0-T355/S36/formal-x86` completed all 428 actions and linked
   `original-softpc-process.exe` with no forced link or warning suppression.

3. One fixed 8-second, console-owning, non-debug observation used only the
   newly linked product and the existing `O:\\ntvdm` immutable media manifest.
   Durable records are:

   ```text
   O:\ntvdm\m0-t355-s36-command-declaration.txt
   O:\ntvdm\m0-t355-s36-command-declaration.txt.json
   ```

   The report records `result=timeout`, the admitted `timeout-ms=8000`, and
   product SHA-256
   `dd7d0992423972eccbfef02e5274272ed691cecbbd333c2e45d3daebba3fb49a`.
   Fixed media and stage-manifest identities remain
   `15b6a215e9bf3cede6650c0990602b0f421181f18ea2ff431031fd237cd82858`
   and
   `cd007a94bd72b0abcc145dd6d27930b3d3c38d1d7d3a144a8a17545e3067758f`.

## Result and limit

The explicit normal declaration, its copied Base VDM record, and its local
consumer contract are source-shaped, compiled, linked and locally tested. The
single integrated observation timed out without an exception or product exit;
it neither proves guest COMMAND execution nor selects a trace-driven repair.
Interactive input, the private BaseSrv/CSRSS producer shell, WOW notification,
PIF, child/parent EXEC, stream duplication and cross-process broker behavior
remain separate owner work.
