# M0 T352 S2 — original `MS_bop_4` return attribution

## Change and source order

The selected original source remains `MS_bop_4` in
`mvdm-host/softpc.new/host/src/nt_bop.c`.  Its existing order is preserved:

1. decode the command service byte;
2. call original `CmdDispatch`;
3. call original `setIP(getIP() + 1)`;
4. emit the new diagnostic record.

The last line is the only S2 production change at the mirror call site.  It
is registered under the existing `MVDM-HOST-DIV-164`.  The adapter record
copies only the already-live fixed-width selector, service, CS:IP, AX, and CF.
It neither reads guest memory nor changes routing, result, IP, flags, session,
CPU, or guest state.

## Formal result

The selected fresh CPU40/x86 graph at
`build/M0-T352/S2/return-observation-x86` generated and completed all 425
actions through `original-softpc-process.exe`.  The product SHA-256 is:

```text
ce4e84dbb2b68af8d949ebfbbb6548e30a2b937c21425d10fbc2c8a103bca6b0
```

The inherited compiler diagnostics remain visible and no warning suppression
was added.  The initially attempted graph generation without its recorded
Node 22 executable failed before writing a graph; rerunning with the required
Node 22 path generated the recorded graph and completed the formal link.

## One fixed-container observation

The linked product was staged once through the existing immutable-media
stager into `build/M0-T352/S2/fixed-container`.  Its manifest SHA-256 is:

```text
cb8572373dbab0264565e52475efb9e1a4f226ef14ce8b7fbdaa29cd4d745d38
```

The existing console-owning, non-debug observer launched one child with its
unchanged `-f -o --ordinary-child` arguments and eight-second watchdog.  The
report SHA-256 is
`8c0feaa89a7c181a8985ef066f1cb918fa37e40b378c642cfe7caa2663d91c290`;
the console snapshot SHA-256 is
`28de9c8bd523e9d06ea52c48f5fc40b53c1e33285c2c257533046024c419cacb`.

The observer ended the child only with its normal timeout result
`0x53504354`.  The console retains `MVDM-BOP-DISPATCH 54:05` but contains no
`MVDM-BOP-RETURN 54:05` record.

## Classification

This proves that, under the one frozen current product/container, execution
has entered original `MS_bop_4` and has **not reached the diagnostic point
after its original `CmdDispatch` plus `setIP(+1)` sequence** before timeout.
It does not prove why: the remaining source-owned region includes the indirect
COMMAND dispatch table/call ABI, `cmdSetInfo`, and its already-admitted scalar
binding boundary.  It explicitly does not authorize a new BOP provider, CCPU,
SAS, BIOS, or guest repair.

S3 is therefore limited to a complete static original COMMAND indirect-call
and `cmdSetInfo` ABI/return audit.  It must establish the earliest wrong or
missing owner edge before any further code change or another observation.
