# M0 T391 S3 — Current COMMAND Worker Binding Confirmation

## Identity

The current product binary is exactly the product used by the retained T390
original-console observation:

- `build/output/ntvdm64-0244.exe`:
  `65375d2f617ad4466e8569f059413a989e5e93fbbbf0a6cc7f6de5aaab44bdc5`
- `O:\ntvdm64\ntvdm.exe`:
  `65375d2f617ad4466e8569f059413a989e5e93fbbbf0a6cc7f6de5aaab44bdc5`

No current source or product-binary distinction invalidates that observation.

## Source-shaped runtime result

The fixed-console evidence in
`m0-t390-s5-original-second-command-console-proof-002.md` is therefore direct
runtime evidence for the current selected COMMAND binding:

1. immutable second `COMMAND.COM` receives `ver` through original DOS CON;
2. it takes original COMMAND native classification and `54:08`;
3. the report records `origin=guest-tail`, followed by the worker command
   `C:\Windows\System32\cmd.exe /c VER`;
4. all worker markers from phase 24 through phase 1 report success;
5. the original guest displays the native child result and returns to the
   original `O:\NTVDM64>` prompt.

This proves the selected binding does not use an app-side parser or launcher:
the existing guest-originated `cmdExec32` worker snapshots its input, obtains
the BaseVDM pending/re-entry interval, starts a child-only Win32 process,
waits for it, and returns control to the original COMMAND continuation.

## S3 result

S3 is closed. The direct adapter bindings are compiled into the formal x86
COMMAND archive (S2), their focused contracts pass (S2), and the exact current
product has already observed the original worker and guest-prompt return.

This does **not** claim all native-child contracts: a DOS-originated child
with non-zero exit code, cancellation, Redirector streams, DOS EXEC/PSP parent
return, `54:0B` ordinary DOS-child return, WOW and multi-process BaseSrv/CSR
remain outside this particular worker proof. S4 must select the narrow
`cmd.exe /c exit 37` lifecycle workload and establish its result at the
current product boundary.
