# M0 T282 S5 — VDM command-acquisition source audit

## Result

Original `GetNextVDMCommand` is a client half of an NT4 BaseSrv/CSR product
protocol, not a direct modern Win32 API and not a COMMAND-private function.
The ledger records seven complete reached slices. The local one-session DOS
request, resize/copy and wait-retry contracts are recoverable through a
same-shaped copied session/adapter-vdm-monitor binding. The first-VDM query
and WOW/multi-process broker branches are not prerequisites for that local
binding and remain explicit later owners.

## Inputs

- `base/win32/client/vdm.c` SHA-256
  `3f03d0dbb08e0163f2d9cf415daad0981e42e1b1855f6f48a3b59022b7374173`.
- `base/win32/server/srvvdm.c` SHA-256
  `c1e2177c6c00679d85cfa475f620841f6736b0e56d8dbf790b71afe33e1ed80b`.
- `base/win32/inc/basemsg.h` SHA-256
  `112b5c45c3c88504f24079a56d8951e24b4068d1b84f41d07c3c5857b32ad023`.

## Interpretation

The next implementation must preserve `VDMINFO` sizing, bounded copy,
`STATUS_INVALID_PARAMETER` size return, second-call state and last-error
failure behavior. It must not import CSR capture buffers, PEB console handles,
native wait handles or server records into MVDM data. No behavior was enabled
by this audit.
