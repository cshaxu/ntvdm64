# T157 S1 post-EOF vector diagnostic design 001

## Decision

No new Bochs intrusion is needed.  The existing registered `BX-TRACE-049`
facility is exactly the smallest evidence source for the T156 uncertainty.

`refs/bochs/cpu/exception.cc:1312-1343` already has the default-off
`BX_NTVDM_ENABLE_REAL_MODE_VECTOR_DIAGNOSTIC` block.  Before original real
mode delivery it snapshots old CS:IP and SS:SP; after the original IVT reads
it logs vector and target CS:IP.  It then performs the unmodified `load_seg`
and `EIP` assignment.

This is already registered as `BX-TRACE-049` in
`docs/etc/research/adapter-external-intrusion-exceptions.md`: generic,
real-mode-only, no extra IVT/RAM access, no BOP/adapter/DOS branch, no state
change and no delivery change.  The existing T155 derivative simply retained
an exception object with this macro disabled.

## Boundary

The next observation may emit all real-mode vector transitions, not an
EOF-special trace.  Selecting only post-EOF vectors inside Bochs would require
stateful BOP/service knowledge and is explicitly rejected.  Correlation is
performed offline by the logged tick sequence after the ordinary `50:42` EOF
resume.  This is both narrower and more faithful to the Bochs/adapter boundary.

## Required later package

T158 may:

1. generate a fresh derivative from T155 inputs;
2. rebuild exactly `cpu/exception.o` with the existing exception intercept,
   BOP listener, bulk-result bridge and `REAL_MODE_VECTOR_DIAGNOSTIC=1`;
3. prove all other retained Bochs objects and archives stay byte-identical;
4. link once and make one separately bounded observation.

It may not add a source patch, rebuild a CPU archive, inspect guest memory,
filter vectors by address/service, alter an exception result, or retry a run.
