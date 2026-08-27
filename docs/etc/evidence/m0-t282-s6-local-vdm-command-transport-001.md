# M0 T282 S6 — local VDM command transport evidence

## Question

Can the single-session DOS portion of original
`GetNextVDMCommand(PVDMINFO)` be recovered without importing BaseSrv/CSR,
retaining caller pointers or handles, or enabling a BOP route?

## Inputs

- `O:\repos.external\OpenNT\base\win32\client\vdm.c`, SHA-256
  `3f03d0dbb08e0163f2d9cf415daad0981e42e1b1855f6f48a3b59022b7374173`.
- `O:\repos.external\OpenNT\base\win32\server\srvvdm.c`, SHA-256
  `c1e2177c6c00679d85cfa475f620841f6736b0e56d8dbf790b71afe33e1ed80b`.
- Exact imported `vdmapi.h`, registered by T282/S2.
- T282/S5 command-acquisition ledger and the admitted
  [S6 plan](../operations/m0-t282-s6-local-vdm-command-transport-plan-001.md).

## Recovery disposition

1. Direct original reuse is not composable: the reached client body creates a
   CSR capture buffer and calls `BasepGetNextVDMCommand`; the server body owns
   console records, duplicated wait objects and cross-process state.
2. The selected smallest same-shaped adapter retains the public entry,
   `VDMINFO` layout, bounded-copy order, capacity failure, environment-only
   branch, re-entry operations and terminal second-call no-command result.
   `app` owns a copied producer record; `session` only binds a synchronous
   dispatch; `adapter-vdm-monitor` owns the public entry/last-error bridge.
3. No Bochs or external-code intrusion was needed.
4. No unrelated new provider was created. The copied producer is the minimum
   source-derived replacement for the unavailable BaseSrv record.

## Procedure

1. Generate x64 and x86 Ninja graphs with
   `tools/build/New-T282LocalVdmCommandNinja.ps1`.
2. Run each graph under its matching MSVC `/MT` developer environment:

   ```text
   ninja -C build/M0-T282/S6/x64 test
   ninja -C build/M0-T282/S6/x86 test
   ```

3. The fixture verifies the original visible cases: unavailable first-VDM
   query; environment-only copied response without consuming the record;
   undersized-buffer `ERROR_INVALID_PARAMETER` and required-size response;
   complete copied command response; terminal
   `RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME` no-command result;
   increment/decrement re-entry accounting; unsupported WOW rejection; and
   session teardown.

## Observations

Both formal graphs compiled, linked and executed successfully:

```text
x64: PASS: local VDM command transport
x86: PASS: local VDM command transport
```

The app record contains only copied byte fields and scalars. The adapter sends
a versioned synchronous wrapper to the bound session dispatch and never stores
the caller's `VDMINFO`, pointers or `HANDLE`s. No static graph includes an
original COMMAND provider, `nt_bop.c`, BOP ingress, Bochs source or an
executable route.

## Interpretation

The admitted one-session local contract is closed. This is not a BaseSrv
replacement: NULL first-VDM query, waiting/retry before the terminal second
call, cross-process console discovery, PIF and WOW are deliberately rejected
with their named future owners. No guest/BOP behavior was enabled.

## Follow-up

Continue T282 only through separately admitted original COMMAND owner slices.
Brokered acquisition remains owned by the later Redirector/broker package;
WOW acquisition remains owned by adapter-wow/broker; PIF remains a later
COMMAND provider slice.
