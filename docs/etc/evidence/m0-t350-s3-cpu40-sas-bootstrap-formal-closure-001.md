# M0 T350 S3 — CPU40/SAS bootstrap formal closure

## Scope

S3 verifies the already admitted original CPU40/SAS execution cohort.  It does
not execute NTDOS, revise a BOP provider, or infer a COMMAND/child-program
result from a machine-only contract.

## Focused owner contract

The existing CPU40 lifecycle fixture was regenerated in
`build/M0-T350/S3/ccpu-lifecycle` and completed successfully after 153 Ninja
actions.  Its output reaches the original lifecycle markers in order:

```text
sas-init
cpu-init
access-init
seed
start
returned-start
reenter
returned-recursive
```

The fixture checks the admitted mechanical contract only:

- original SAS RAM initialization and bounded scalar/move/fill access;
- original CPU40 access-vector installation through
  `load_sw_cpu_access_functions`;
- direct `host_start_cpu` entry and the original controlled return;
- recursive `host_simulate` re-entry and session termination cleanup; and
- the existing session mapping-manager physical-memory bridge, without a raw
  guest-pointer alias or a second mapper.

The initial `ccpu386ThrdExptnPtr ... Bad Id` diagnostic is the fixture's
intentional negative check for an uninitialized exception-frame context; the
fixture exits successfully only after that decline and the lifecycle sequence
above.

## Formal selected product link

A fresh CPU40/Win32/x86 graph was generated at
`build/M0-T350/S3/record-x86` and linked
`original-softpc-process.exe` successfully in 425 Ninja actions.  This is the
selected final product closure for the unchanged T350 source cohort.

Original-source warnings remain visible in that build.  S3 added no warning
suppression and no source-derived behavior.  In particular, a successful link
does not establish that the guest has completed `SVC_CMDSETINFO`, entered its
new DOS stack, or executed a COMMAND child.

## Frozen S4 input

S4 may stage this exact formal product beside the already selected original DOS
and firmware media, then launch it once with the established non-debug,
console-owning observer and `-f -o --ordinary-child` contract.  It may classify
only the next source-defined ingress or terminal boundary; it cannot select a
new BOP repair.

