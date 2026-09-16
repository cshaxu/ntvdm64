# M0 T418 — three-program component normalization closure

T418 closes the owner-approved transition to an executable-owned standalone
NTVDM product:

```text
src/run16/   -> run16.exe
src/basesrv/ -> basesrv.exe
src/ntvdm/   -> ntvdm.exe
```

S1 deleted the unreachable local broker archive. S2 placed launcher and
stateless product version/package code at their final owners. S3 placed the
authenticated BaseSrv endpoint and transport in `basesrv`, while retaining
original `srvvdm.c` as the record-policy owner in `opennt-host`. S4 moved the
worker-local session and interface bindings to `ntvdm`. S5 established the
final shared-code rule, regenerated the formal x86 package, and passed the
original-owner, runtime and failure-lifecycle matrix.

No generic `common`, `win32api`, session manager or broker component remains.
An original algorithm remains in `mvdm` or `opennt-host`; BaseSrv protocol code
is basesrv-owned; process-local Win32 integration stays with its executable;
and the only reusable historical-host binding island is the named,
source-shaped `opennt-abi/host-compat` closure. Product-wide sharing remains
limited to stateless `product-abi` and `product-package` data.

The final evidence, exact fresh build root, runtime log names, fixture results
and limits are recorded in the [S5 closure evidence](../etc/evidence/m0-t418-s5-three-program-final-closure.md).
The deployed formal package is `O:\winnt\{run16,basesrv,ntvdm}.exe` plus
`VDMREDIR.dll`. T418 does not claim WOW16 completion; the ordered queue owns
that later recovery work.

## S6--S7 footprint supplement

The owner subsequently reopened T418 for a bounded non-mirror footprint audit
and only its source-proven immediate retirements.  S6 established a 74-input,
12,427-physical-line formal non-mirror baseline.  S7 removed the unused
264-line duplicate classifier from the selected graph, plus 28 lines of
unselected duplicate/stub C bodies and their declarations.  The resulting
formal closure is 73 inputs and 12,163 physical lines.  The tested formal
x86 package is deployed to `O:\winnt`; details, retained fixture-only sources
and the one unavailable hidden-observer interaction are recorded in the
[S7 evidence](../etc/evidence/m0-t418-s7-proven-dead-code-retirement.md).
