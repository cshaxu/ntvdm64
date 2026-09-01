# M0 T352 S1 — post-`SVC_CMDSETINFO` owner admission

## Inputs

- T351's fixed, non-debug CPU40/x86 observation reaches original `54:05` and
  then times out under its immutable eight-second container.
- The current selected mirrors of `nt_bop.c`, COMMAND dispatch and `msinit.asm`.
- The CPU40 source entry and BOP decode path in `c_main.c`.

## Source result

`msinit.asm` invokes `SVC_CMDSETINFO` with three SCS scalar locations.  The
original CPU40 BOP decode has already advanced the Intel IP to that service
byte before it invokes `MS_bop_4`.  `MS_bop_4` then reads the byte,
`CmdDispatch` selects original `cmdSetInfo`, and the original handler returns
without replacing the CPU frame or stack.  `MS_bop_4` must finally consume the
service byte with `setIP(getIP() + 1)`.  The immediately following guest code
pops its original BX/CX/DX values and starts BIOS/DOS stack exchange.

The earlier ingress-only observer proves none of the return edge.  It cannot
distinguish a non-returning provider from a completed provider followed by a
CPU40/SAS/guest failure.

## Recovery-rung disposition

1. **Original source:** retained directly for `c_main.c`, `MS_bop_4`,
   `cmddisp.c`, `cmdmisc.c`, and `msinit.asm`.
2. **Existing smallest facade:** T351's scalar binding is the only reached
   facade; it preserves numeric 16:16 SCS locations and takes no durable raw
   guest pointer.
3. **Intrusion:** the already registered `MVDM-HOST-DIV-164` diagnostic seam
   is the smallest usable observation boundary.  Its proposed extension is
   observational only and does not select, route, alter, or fail a BOP.
4. **New behavior:** none is admitted.

## Selected S2 cohort

S2 may extend the existing diagnostic seam with a source-shaped
post-`CmdDispatch` record in `MS_bop_4`, immediately before the original
`setIP`.  It may copy only already-live fixed-width values needed to establish
return attribution: selector, service, CS:IP, AX and flags.  It must not read
guest memory, retain a guest pointer, write CPU/guest/session state, change
the `CmdDispatch` result, change IP order, or add a COMMAND provider branch.

This is the earliest complete owner cohort.  CCPU unsimulation, SAS, the
`msinit.asm` pops, and FDC/INT15/timer/PIT/ICA remain source-identified sibling
boundaries, but are not selected until the post-dispatch marker demonstrates
that the original host handler returned.

The full per-edge disposition is recorded in
[the S1 owner ledger](../operations/m0-t352-s1-post-cmdsetinfo-owner-ledger.tsv).

## Verification and limitation

This S is a source/graph review only.  It ran no new container, build, or
guest trace.  Therefore it establishes the only admissible S2 observation
boundary but makes no claim that `cmdSetInfo`, `setIP`, CPU40 resume, or NTDOS
guest continuation has occurred at runtime.
